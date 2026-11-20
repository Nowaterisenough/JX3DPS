//! Standalone port of the default Tai Xu rotation. Input tables are exported
//! at build time. No C++ code is linked into this executable.
mod queue;
use queue::Queue;
use std::{env, fs, thread, time::Instant};
const NONE: u16 = u16::MAX;
const WUWO: usize = 0;
const BAHUANG: usize = 1;
const SANHUAN: usize = 2;
const SANCHAI: usize = 3;
const ZIQI: usize = 4;
const PURPLE: u16 = 1;
#[derive(Clone, Copy)]
struct Formula {
    skill: u16,
    sub: u16,
    level: u16,
    kind: u8,
    fixed: i64,
    weapon: i64,
    count: i64,
    coefficient: f64,
    surplus: i64,
    bonus: i64,
    power: i64,
}
#[derive(Clone, Copy)]
struct Attributes {
    attack: i64,
    weapon: i64,
    bonus: i64,
    mitigation: i64,
    power: i64,
    strain: i64,
    pve: i64,
    vulnerable: i64,
    chance: f64,
}
include!(env!("TX_DEFAULT_DATA"));

fn mix(mut x: u64) -> u64 {
    x = (x ^ (x >> 30)).wrapping_mul(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)).wrapping_mul(0x94d049bb133111eb);
    x ^ (x >> 31)
}
const STEP: u64 = 0x9e3779b97f4a7c15;
fn seed(base: u64, i: u64) -> u64 {
    mix(base.wrapping_add(STEP.wrapping_mul(i + 1)))
}

#[derive(Clone, Copy, Default)]
#[repr(C)]
struct Hit {
    frame: i32,
    seq: u32,
    skill: u16,
    target: u16,
    level: u16,
    outcome: i32,
    key: u64,
    base: i64,
    coefficient: f64,
    effect: u16,
    profile: u16,
    sub: u16,
    frozen: u32,
    live: u32,
}
#[derive(Clone, Copy)]
#[repr(C)]
struct Mutation {
    frame: i32,
    seq: u32,
    kind: u8,
    slot: u16,
    target: bool,
    a: i32,
    b: i32,
    c: i32,
}
#[derive(Clone, Copy, Default)]
struct CacheEntry {
    generation: u32,
    version: u32,
    effect_roll: u32,
    damage: i64,
}
#[derive(Clone, Copy, Default)]
struct Skill {
    damage: i64,
    counts: [u64; 4],
}
#[derive(Default)]
struct Totals {
    skills: [Skill; 19],
    checksum: u64,
}
macro_rules! emit { ($buf:expr,$($x:expr),+)=>{{ $($buf.extend_from_slice(&$x.to_le_bytes());)+ }} }

struct Fight {
    q: Queue,
    cd_timer: [usize; 5],
    gcd_timer: usize,
    regen: usize,
    tick: usize,
    expire: usize,
    ready: [i32; 5],
    gcd: i32,
    points: i32,
    charges: i32,
    recharge: i32,
    purple_end: i32,
    version: u32,
    profiles: Vec<u16>,
    rng: u64,
    seq: u32,
    last_frame: i32,
    hits: Vec<Hit>,
    mutations: Vec<Mutation>,
    cache: [CacheEntry; 1024],
    generation: u32,
}
impl Fight {
    fn new() -> Self {
        let id = |kind, slot| {
            TIMERS
                .iter()
                .position(|t| t.0 == kind && t.1 == slot && !t.2)
                .unwrap()
        };
        let priorities = TIMERS.iter().map(|t| t.3).collect::<Vec<_>>();
        Self {
            q: Queue::new(&priorities),
            cd_timer: std::array::from_fn(|i| id(0, i as u16)),
            gcd_timer: id(5, NONE),
            regen: id(7, NONE),
            tick: id(4, PURPLE),
            expire: id(3, PURPLE),
            ready: [0; 5],
            gcd: 0,
            points: 10,
            charges: 2,
            recharge: 0,
            purple_end: 0,
            version: 0,
            profiles: Vec::with_capacity(4096),
            rng: 0,
            seq: 0,
            last_frame: 0,
            hits: Vec::with_capacity(4096),
            mutations: Vec::with_capacity(8192),
            cache: [CacheEntry::default(); 1024],
            generation: 0,
        }
    }
    fn sequence(&mut self) -> u32 {
        assert!(self.last_frame <= self.q.now);
        self.last_frame = self.q.now;
        let seq = self.seq;
        self.seq = self.seq.checked_add(1).unwrap();
        seq
    }
    fn mutation(&mut self, kind: u8, slot: u16, a: i32, b: i32, c: i32) {
        assert!(self.mutations.len() < 8192);
        let seq = self.sequence();
        self.mutations.push(Mutation {
            frame: self.q.now,
            seq,
            kind,
            slot,
            target: false,
            a,
            b,
            c,
        });
    }
    fn qidian(&mut self, new: i32) {
        self.points = new.clamp(0, 10);
        self.mutation(2, NONE, self.points, 0, 0);
    }
    fn cooldown(&mut self, skill: usize, at: i32) {
        self.ready[skill] = at;
        self.mutation(1, skill as u16, at, 0, 0);
        self.q.arm(self.cd_timer[skill], at);
    }
    fn hit(&mut self, effect: u16) {
        let f = FORMULAS[effect as usize];
        let profile = u16::from(self.purple_end != 0);
        if self.profiles.last() != Some(&profile) {
            assert!(self.profiles.len() < 4096);
            self.version = self.profiles.len() as u32;
            self.profiles.push(profile);
        }
        let (key, outcome) = if f.kind == 3 {
            (0, 0)
        } else {
            self.rng = self.rng.wrapping_add(STEP);
            let k = mix(self.rng);
            (
                k,
                i32::from(
                    ((k >> 11) as f64) * (1.0 / (1u64 << 53) as f64)
                        < ATTRS[profile as usize].chance,
                ),
            )
        };
        assert!(self.hits.len() < 4096);
        let seq = self.sequence();
        self.hits.push(Hit {
            frame: self.q.now,
            seq,
            skill: f.skill,
            target: if f.kind == 3 { NONE } else { 0 },
            level: f.level,
            outcome,
            key,
            base: 0,
            coefficient: 0.0,
            effect,
            profile,
            sub: f.sub,
            frozen: self.version,
            live: self.version,
        });
    }
    fn timer(&mut self, id: usize) {
        let now = self.q.now;
        if id == self.regen {
            if self.points < 10 {
                self.qidian(self.points + 1);
            }
            self.q.arm(id, now + 16);
        } else if id == self.tick {
            self.qidian(self.points + 2);
            self.mutation(0, PURPLE, self.purple_end, now + 16, 1);
            self.q.arm(id, now + 16);
        } else if id == self.expire {
            self.purple_end = 0;
            self.mutation(0, PURPLE, 0, 0, 0);
            self.q.cancel(self.tick);
        } else if id == self.cd_timer[ZIQI] {
            self.charges += 1;
            self.mutation(4, ZIQI as u16, self.charges, 0, 0);
            if self.charges < 2 {
                self.recharge = now + 1200;
                self.q.arm(id, self.recharge);
            } else {
                self.recharge = 0;
            }
            self.ready[ZIQI] = 0;
            self.mutation(1, ZIQI as u16, 0, 0, 0);
        }
    }
    fn cast(&mut self, line: usize, skill: usize) -> bool {
        let now = self.q.now;
        if self.ready[skill] > now || (skill <= SANHUAN && self.gcd > now) {
            return false;
        }
        match line {
            0 if self.points >= 6 || self.purple_end != 0 || self.charges == 0 => return false,
            1 if self.points < 8 => return false,
            5 if self.points < 1 => return false,
            _ => {}
        }
        if skill <= SANHUAN {
            self.gcd = now + 24;
            self.mutation(3, 0, self.gcd, 0, 0);
            self.q.arm(self.gcd_timer, self.gcd);
        }
        match skill {
            ZIQI => {
                if self.charges == 2 {
                    self.recharge = now + 1200;
                    self.q.arm(self.cd_timer[ZIQI], self.recharge);
                }
                self.charges -= 1;
                self.mutation(4, ZIQI as u16, self.charges, 0, 0);
                self.ready[ZIQI] = if self.charges > 0 { 0 } else { self.recharge };
                self.mutation(1, ZIQI as u16, self.ready[ZIQI], 0, 0);
                self.qidian(10);
                self.purple_end = now + 160;
                self.mutation(0, PURPLE, self.purple_end, now + 16, 1);
                self.q.arm(self.tick, now + 16);
                self.q.arm(self.expire, self.purple_end);
                self.hit(99);
            }
            WUWO => {
                let qi = self.points;
                self.qidian(0);
                self.hit((qi - 1) as u16);
                if qi >= 6 {
                    self.hit((33 + (qi - 6) / 2) as u16);
                }
            }
            BAHUANG => {
                self.cooldown(skill, now + 192);
                self.qidian(self.points + 2);
                self.hit(20);
                self.hit(21);
            }
            SANHUAN => {
                self.cooldown(skill, now + 32);
                self.qidian(self.points + 2);
                for _ in 0..3 {
                    self.hit(31);
                }
            }
            SANCHAI => {
                self.cooldown(skill, now + 22);
                self.hit(32);
            }
            _ => unreachable!(),
        }
        true
    }
    fn fight<const TRACE: bool>(&mut self, iteration: u64, out: &mut Totals, bytes: &mut Vec<u8>) {
        self.q.reset();
        self.ready.fill(0);
        self.gcd = 0;
        self.points = 10;
        self.charges = 2;
        self.recharge = 0;
        self.purple_end = 0;
        self.profiles.clear();
        self.hits.clear();
        self.mutations.clear();
        self.seq = 0;
        self.last_frame = 0;
        self.generation += 1;
        self.rng = seed(42, iteration);
        self.q.arm(
            self.regen,
            (mix((self.rng ^ 0x6a09e667f3bcc909).wrapping_add(STEP)) & 15) as i32,
        );
        if TRACE {
            emit!(bytes, b'F', iteration);
        }
        loop {
            while let Some(id) = self.q.pop_due() {
                if TRACE {
                    let t = TIMERS[id];
                    emit!(bytes, b'T', self.q.now, id as u32, t.0, t.1, t.2 as u8);
                }
                self.timer(id);
            }
            let mut success = false;
            for (line, skill) in [ZIQI, WUWO, BAHUANG, SANHUAN, SANCHAI, WUWO]
                .into_iter()
                .enumerate()
            {
                if self.cast(line, skill) {
                    if TRACE {
                        emit!(bytes, b'C', self.q.now, line as u32, skill as u16, 0u8);
                    }
                    success = true;
                    break;
                }
            }
            if success {
                continue;
            }
            let next = self.q.next().unwrap_or(i32::MAX);
            if next >= 4800 {
                break;
            }
            self.q.now = next;
        }
        std::hint::black_box((&self.hits, &self.mutations));
        let mut sum = 0;
        for h in &self.hits {
            let effect_roll = h.effect as u32 * 2 + h.outcome as u32;
            let index = (h.frozen.wrapping_mul(0x9e3779b1)
                ^ h.live.wrapping_mul(0x85ebca6b)
                ^ effect_roll * 17)
                & 1023;
            let entry = &mut self.cache[index as usize];
            if (entry.generation, entry.version, entry.effect_roll)
                != (self.generation, h.frozen, effect_roll)
            {
                let f = FORMULAS[h.effect as usize];
                let a = ATTRS[self.profiles[h.frozen as usize] as usize];
                let mut raw = if f.kind == 1 {
                    f.surplus
                } else {
                    f.fixed + (a.attack as f64 * f.coefficient) as i64 + a.weapon * f.weapon / 1024
                };
                raw = raw * (1024 + a.bonus + f.bonus) / 1024 * f.count * a.mitigation / 1024;
                if h.outcome == 1 {
                    raw = (raw as f64
                        + raw as f64 * 0.75
                        + (raw * (a.power + f.power) / 1024) as f64)
                        as i64;
                }
                raw = (raw as f64 * LEVEL) as i64;
                let damage = if f.kind == 3 {
                    0
                } else {
                    raw * a.strain / 1024 * (1024 + a.pve) / 1024 * (1024 + a.vulnerable) / 1024
                };
                *entry = CacheEntry {
                    generation: self.generation,
                    version: h.frozen,
                    effect_roll,
                    damage,
                };
            }
            let damage = entry.damage;
            sum += damage;
            let s = &mut out.skills[h.skill as usize];
            s.damage += damage;
            s.counts[h.outcome as usize] += 1;
            if TRACE {
                emit!(
                    bytes,
                    b'H',
                    h.frame,
                    h.seq,
                    h.skill,
                    h.target,
                    h.level,
                    h.outcome,
                    h.key,
                    h.base,
                    h.coefficient,
                    h.effect,
                    h.profile,
                    h.sub,
                    h.frozen,
                    h.live,
                    damage
                );
            }
        }
        if TRACE {
            for m in &self.mutations {
                emit!(
                    bytes,
                    b'M',
                    m.frame,
                    m.seq,
                    m.kind,
                    m.slot,
                    m.target as u8,
                    m.a,
                    m.b,
                    m.c
                );
            }
        }
        out.checksum ^= seed(sum as u64, iteration);
    }
}
#[link(name = "kernel32")]
unsafe extern "system" {
    fn GetCurrentThread() -> isize;
    fn SetThreadAffinityMask(thread: isize, mask: usize) -> usize;
}
fn main() {
    let args: Vec<_> = env::args().collect();
    let n: u64 = args[1].parse().unwrap();
    let w: u64 = args[2].parse().unwrap();
    assert!(w > 0 && n >= w && n <= u32::MAX as u64);
    let audit = args.get(3);
    if w == 1 {
        assert_ne!(unsafe { SetThreadAffinityMask(GetCurrentThread(), 4) }, 0);
    } else {
        assert!(audit.is_none());
    }
    let start = Instant::now();
    let mut total = Totals::default();
    thread::scope(|scope| {
        let work = |j| {
            let mut fight = Fight::new();
            let mut result = Totals::default();
            let mut bytes = Vec::new();
            for i in n * j / w..n * (j + 1) / w {
                if audit.is_some() {
                    fight.fight::<true>(i, &mut result, &mut bytes);
                } else {
                    fight.fight::<false>(i, &mut result, &mut bytes);
                }
            }
            if let Some(path) = audit {
                fs::write(path, bytes).unwrap();
            }
            result
        };
        let threads: Vec<_> = (1..w).map(|j| scope.spawn(move || work(j))).collect();
        for r in std::iter::once(work(0)).chain(threads.into_iter().map(|t| t.join().unwrap())) {
            total.checksum ^= r.checksum;
            for (s, other) in total.skills.iter_mut().zip(r.skills) {
                s.damage += other.damage;
                for i in 0..4 {
                    s.counts[i] += other.counts[i];
                }
            }
        }
    });
    print!("{} {} {}", start.elapsed().as_secs_f64(), n, total.checksum);
    for s in total.skills {
        print!(" {}", s.damage);
        for c in s.counts {
            print!(",{}", c);
        }
    }
    println!();
}
