mod rotation;
use rotation::Program;
// Independent four-scenario combat benchmark. No C++ code, input traces or FFI.
mod catalog;
mod mo_wen;
#[path = "../queue.rs"]
mod queue;
mod tai_xu;
use queue::Queue;
use std::{env, fs, thread, time::Instant};
const NONE: u16 = u16::MAX;
const COOLDOWN: u8 = 0;
const PREPARE: u8 = 1;
const CHANNEL: u8 = 2;
const EXPIRE: u8 = 3;
const TICK: u8 = 4;
const GCD: u8 = 5;
const FIELD: u8 = 6;
const REGEN: u8 = 7;
const TEAM: u8 = 8;

#[derive(Clone, Copy)]
struct Timer(u8, u16, bool, u16);
// skill, sub, level, fixed damage, weapon factor, channel coefficient, count,
// formula kind (direct/surplus/DOT/zero), DOT count and interval.
#[derive(Clone, Copy)]
struct Raw(u16, u16, u16, i64, i64, i64, i64, u8, i64, i64);
impl Raw {
    fn base(self, attack: i64, weapon: i64, divisor: f64) -> i64 {
        match self.7 {
            3 => 0,
            1 => {
                ((3000 * ((self.5 + i64::from(self.5 < 0)) / 1024 + 1024) / 1024) as f64 * 13.192)
                    as i64
            }
            _ => {
                let coeff = if self.7 == 2 {
                    (self.5 * (self.8 * self.9 / 12).max(16)) as f64
                        / self.8 as f64
                        / divisor
                        / 16.0
                        / 16.0
                } else {
                    self.5.max(16) as f64 / divisor / 16.0
                };
                self.3 + (attack as f64 * coeff) as i64 + weapon * self.4 / 1024
            }
        }
    }
}
fn mitigation(overcome: i64, shield: i64) -> i64 {
    let factor = (overcome as f64 * 1024.0 / (9.530 * 8250.0) + 1024.0) as i64;
    let reduction =
        (shield as f64 * 1024.0 / (shield as f64 + 5.091 * (450.0 * 124.0 - 45750.0))) as i64;
    factor - factor * reduction / 1024
}
fn finish(mut raw: i64, critical: bool, power: i64, strain: i64) -> i64 {
    if critical {
        raw = (raw as f64 + raw as f64 * 0.75 + (raw * power / 1024) as f64) as i64;
    }
    (raw as f64 * (1.0 - 4.0 * 0.05)) as i64 * strain / 1024
}
const STEP: u64 = 0x9e3779b97f4a7c15;
fn mix(mut x: u64) -> u64 {
    x = (x ^ (x >> 30)).wrapping_mul(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)).wrapping_mul(0x94d049bb133111eb);
    x ^ (x >> 31)
}
fn seed(base: u64, i: u64) -> u64 {
    mix(base.wrapping_add(STEP.wrapping_mul(i + 1)))
}
fn unit(k: u64) -> f64 {
    ((k >> 11) as f64) * (1.0 / (1u64 << 53) as f64)
}
#[derive(Clone, Copy, Default)]
struct Buff {
    end: i32,
    next: i32,
    n: i32,
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
struct Core {
    q: Queue,
    timers: &'static [Timer],
    ids: [[usize; 65]; 9],
    hits: Vec<Hit>,
    mutations: Vec<Mutation>,
    seq: u32,
    last: i32,
    rng: u64,
    buffs: [Buff; 48],
    ready: [i32; 32],
    energy: [i32; 32],
    gcd: [i32; 8],
    last_skill: u16,
    now: i32,
}
impl Core {
    fn new(timers: &'static [Timer]) -> Self {
        let mut ids = [[usize::MAX; 65]; 9];
        for (i, t) in timers.iter().enumerate() {
            ids[t.0 as usize][(t.1 as usize).min(64)] = i;
        }
        Self {
            q: Queue::new(&timers.iter().map(|t| t.3).collect::<Vec<_>>()),
            timers,
            ids,
            hits: Vec::with_capacity(4096),
            mutations: Vec::with_capacity(8192),
            seq: 0,
            last: 0,
            rng: 0,
            buffs: [Buff::default(); 48],
            ready: [0; 32],
            energy: [0; 32],
            gcd: [0; 8],
            last_skill: NONE,
            now: 0,
        }
    }
    fn reset(&mut self, rng: u64) {
        self.q.reset();
        self.last_skill = NONE;
        self.hits.clear();
        self.mutations.clear();
        self.seq = 0;
        self.last = 0;
        self.rng = rng;
        self.now = 0;
        self.buffs.fill(Buff::default());
        self.ready.fill(0);
        self.energy.fill(0);
        self.gcd.fill(0);
    }
    fn random(&mut self) -> u64 {
        self.rng = self.rng.wrapping_add(STEP);
        mix(self.rng)
    }
    fn chance(&mut self, n: u64, d: u64) -> bool {
        self.random() % d < n
    }
    fn arm(&mut self, k: u8, s: u16, at: i32) {
        self.q.arm(self.ids[k as usize][(s as usize).min(64)], at);
    }
    fn cancel(&mut self, k: u8, s: u16) {
        self.q.cancel(self.ids[k as usize][(s as usize).min(64)]);
    }
    fn seq(&mut self) -> u32 {
        assert!(self.last <= self.now);
        self.last = self.now;
        self.seq += 1;
        self.seq - 1
    }
    fn change(&mut self, kind: u8, slot: u16, target: bool, a: i32, b: i32, c: i32) {
        assert!(self.mutations.len() < 8192);
        let seq = self.seq();
        self.mutations.push(Mutation {
            frame: self.now,
            seq,
            kind,
            slot,
            target,
            a,
            b,
            c,
        });
    }
    fn record(&mut self, mut h: Hit) {
        assert!(self.hits.len() < 4096);
        h.seq = self.seq();
        h.frame = self.now;
        self.hits.push(h);
    }
    fn buff(&mut self, s: u16, target: bool, end: i32, next: i32, n: i32) {
        self.buffs[s as usize] = Buff { end, next, n };
        self.change(0, s, target, end, next, n);
    }
    fn stacks(&self, s: u16) -> i32 {
        self.buffs[s as usize].n
    }
    fn cd(&mut self, s: u16, at: i32) {
        self.ready[s as usize] = at;
        self.change(1, s, false, at, 0, 0);
    }
    fn charge(&mut self, s: u16, n: i32) {
        self.energy[s as usize] = n;
        self.change(4, s, false, n, 0, 0);
    }
    fn gcd(&mut self, s: u16, at: i32) {
        self.gcd[s as usize] = at;
        self.change(3, s, false, at, 0, 0);
    }
}
trait Rules {
    const TIMERS: &'static [Timer];
    const SKILLS: usize;
    fn new(advanced: bool) -> Self;
    fn reset(&mut self, c: &mut Core);
    fn timer(&mut self, c: &mut Core, t: Timer);
    fn ready(&self, c: &Core, skill: u16) -> i32;
    fn qi(&self) -> i32 {
        0
    }
    fn busy(&self) -> bool {
        false
    }
    fn interrupt(&mut self, _c: &mut Core) {}
    fn available(&self, c: &Core, s: u16) -> bool {
        !self.busy() && self.ready(c, s) <= c.now
    }
    fn perform(&mut self, c: &mut Core, skill: u16) -> bool;
    fn reduce(&mut self, h: &Hit) -> i64;
}
#[derive(Default)]
struct Stats {
    sum: [i64; 32],
    outcomes: [[u64; 4]; 32],
    check: u64,
}
macro_rules! emit{($v:expr,$($x:expr),+)=>{{$($v.extend_from_slice(&$x.to_le_bytes());)+}}}
fn fight<R: Rules, const TRACE: bool>(
    program: &Program,
    consumed: &mut [bool],
    r: &mut R,
    c: &mut Core,
    i: u64,
    s: &mut Stats,
    bytes: &mut Vec<u8>,
) {
    consumed.fill(false);
    c.reset(seed(42, i));
    r.reset(c);
    c.hits.clear();
    c.mutations.clear();
    c.seq = 0;
    c.last = 0;
    if TRACE {
        emit!(bytes, b'F', i);
    }
    loop {
        while let Some(id) = c.q.pop_due() {
            let t = c.timers[id];
            if TRACE {
                emit!(bytes, b'T', c.now, id as u32, t.0, t.1, t.2 as u8);
            }
            r.timer(c, t);
        }
        if let Some((line, skill, command)) = program.scan(r, c, consumed) {
            if TRACE {
                emit!(bytes, b'C', c.now, line, skill, command);
            }
            continue;
        }
        let next = program.next(r, c, c.q.next().unwrap_or(i32::MAX));
        if next >= 4800 {
            break;
        }
        c.now = next;
        c.q.now = next;
    }
    std::hint::black_box((&c.hits, &c.mutations));
    let mut total = 0;
    for h in &c.hits {
        let d = r.reduce(h);
        total += d;
        s.sum[h.skill as usize] += d;
        s.outcomes[h.skill as usize][h.outcome as usize] += 1;
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
                d
            );
        }
    }
    if TRACE {
        for m in &c.mutations {
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
    s.check ^= seed(total as u64, i);
}
#[cfg(windows)]
#[link(name = "kernel32")]
unsafe extern "system" {
    fn GetCurrentThread() -> isize;
    fn SetThreadAffinityMask(t: isize, mask: usize) -> usize;
}
fn run<R: Rules>(program: &Program, advanced: bool, n: u64, w: u64, trace: Option<&String>) {
    let mut totals = Stats::default();
    let start = Instant::now();
    let work = |j| {
        let mut r = R::new(advanced);
        let mut c = Core::new(R::TIMERS);
        let mut s = Stats::default();
        let mut bytes = Vec::new();
        let mut consumed = vec![false; program.len()];
        for i in n * j / w..n * (j + 1) / w {
            if trace.is_some() {
                fight::<R, true>(
                    program,
                    &mut consumed,
                    &mut r,
                    &mut c,
                    i,
                    &mut s,
                    &mut bytes,
                );
            } else {
                fight::<R, false>(
                    program,
                    &mut consumed,
                    &mut r,
                    &mut c,
                    i,
                    &mut s,
                    &mut bytes,
                );
            }
        }
        if let Some(path) = trace {
            fs::write(path, bytes).unwrap();
        }
        s
    };
    thread::scope(|scope| {
        let workers: Vec<_> = (1..w).map(|j| scope.spawn(move || work(j))).collect();
        for s in std::iter::once(work(0)).chain(workers.into_iter().map(|t| t.join().unwrap())) {
            totals.check ^= s.check;
            for j in 0..R::SKILLS {
                totals.sum[j] += s.sum[j];
                for k in 0..4 {
                    totals.outcomes[j][k] += s.outcomes[j][k];
                }
            }
        }
    });
    print!("{} {} {}", start.elapsed().as_secs_f64(), n, totals.check);
    for j in 0..R::SKILLS {
        print!(" {}", totals.sum[j]);
        for k in totals.outcomes[j] {
            print!(",{}", k);
        }
    }
    println!();
}
fn main() {
    let args: Vec<_> = env::args().collect();
    assert!(
        (4..=6).contains(&args.len()),
        "SCENE FIGHTS WORKERS [TRACE [MACRO]]"
    );
    let n = args[2].parse().unwrap();
    let w = args[3].parse().unwrap();
    assert!(w > 0 && n >= w && n <= u32::MAX as u64 && (args.len() == 4 || w == 1));
    #[cfg(windows)]
    if w == 1 {
        assert_ne!(unsafe { SetThreadAffinityMask(GetCurrentThread(), 4) }, 0);
    }
    let tx = args[1].starts_with("tx");
    let text = if let Some(path) = args.get(5) {
        fs::read_to_string(path).unwrap()
    } else if tx {
        rotation::TX_MACRO.to_owned()
    } else {
        rotation::MW_MACRO.to_owned()
    };
    let program = Program::compile(tx, &text).unwrap();
    match args[1].as_str() {
        "tx" | "tx_buffs" => {
            run::<tai_xu::TaiXu>(&program, args[1] == "tx_buffs", n, w, args.get(4))
        }
        "mw" | "mw_equipped" => {
            let equipped = args[1] == "mw_equipped";
            mo_wen::prepare(equipped);
            run::<mo_wen::MoWen>(&program, equipped, n, w, args.get(4));
        }
        _ => panic!("invalid scene"),
    }
}
