use super::*;
const WUWO: u16 = 0;
const BAHUANG: u16 = 1;
const SANHUAN: u16 = 2;
const SANCHAI: u16 = 3;
const ZIQI: u16 = 4;
const DIEREN: u16 = 0;
const PURPLE: u16 = 1;
const LIEYUN: u16 = 11;
const HIDDEN: u16 = 12;
const WEAPON: u16 = 19;
const WANXIANG: u16 = 20;
const SET: u16 = 21;
const POFENG: u16 = 24;
const JILEI: u16 = 33;
const NONGMEI: u16 = 34;
const DRUM: u16 = 36;
const SCHEDULE: [(i32, u16, i32, i32); 4] = [
    (0, JILEI, 96, 1),
    (16, DRUM, 960, 48),
    (32, POFENG, 160, 1),
    (48, NONGMEI, 96, 1),
];
#[derive(Clone, Copy)]
struct Version {
    profile: u16,
    team: u32,
    attack: i64,
    power: i64,
    mitigation: i64,
    strain: i64,
    chance: f64,
}
#[derive(Clone, Copy, Default)]
struct Cache {
    generation: u32,
    frozen: u32,
    live: u32,
    roll: u32,
    damage: i64,
}
const SUIXING: u16 = 7;
const SHENGTAI: u16 = 8;
const RENJIAN: u16 = 12;
const SUIXING_BUFF: u16 = 3;
const XUANMEN: u16 = 10;
#[derive(Clone, Copy, Default)]
struct FieldState {
    skill: u16,
    end: i32,
    next: i32,
    order: u32,
}
pub struct TaiXu {
    fields: [FieldState; 3],
    field_order: u32,
    preparing: bool,
    team: bool,
    qi: i32,
    recharge: i32,
    weapon_ready: i32,
    profile: u16,
    team_version: u32,
    cursor: usize,
    dot: u32,
    wan: u32,
    versions: Vec<Version>,
    generation: u32,
    cache: [Cache; 1024],
}
impl TaiXu {
    fn snapshot(&mut self, c: &Core) -> u32 {
        if self
            .versions
            .last()
            .is_none_or(|v| v.profile != self.profile || v.team != self.team_version)
        {
            let ji = c.stacks(JILEI) as i64;
            let mei = c.stacks(NONGMEI) as i64;
            let purple = i64::from(self.profile & 1 != 0);
            let attack = 10000 * (1024 + 205 * ji + 256 * purple) / 1024 + 2500;
            let overcome = if self.team {
                (20000 + 700 * mei) * (1024 + 205 * ji + 204 * ((self.profile as i64 & 96) / 32))
                    / 1024
                    + 5000
            } else {
                25000
            };
            let shield = 30000 - 1150 * c.stacks(POFENG) as i64;
            let power = (15000.0 * 1024.0 / (3.335 * 8250.0)) as i64
                + 256 * purple
                + if self.profile & 2 != 0 { 102 } else { 0 }
                + if self.profile & 16 != 0 { 154 } else { 0 }
                + if self.profile & 128 != 0 { 41 } else { 0 };
            let strain =
                ((10000 + 440 * c.stacks(DRUM)) as f64 * 1024.0 / (9.189 * 8250.0)) as i64 + 1024;
            let chance = 20000.0 / (9.530 * 8250.0)
                + (2500 * purple
                    + if self.profile & 2 != 0 { 500 } else { 0 }
                    + 300 * ((self.profile as i64 & 96) / 32)
                    + if self.profile & 128 != 0 { 400 } else { 0 }) as f64
                    / 10000.0;
            assert!(self.versions.len() < 4096);
            self.versions.push(Version {
                profile: self.profile,
                team: self.team_version,
                attack,
                power,
                mitigation: mitigation(
                    overcome,
                    shield
                        - shield * (205 * mei + if self.profile & 8 != 0 { 614 } else { 0 }) / 1024,
                ),
                strain,
                chance,
            });
        }
        (self.versions.len() - 1) as u32
    }
    fn qi(&mut self, c: &mut Core, n: i32) {
        self.qi = n.clamp(0, 10);
        c.change(2, NONE, false, self.qi, 0, 0);
    }
    fn roll(&self, c: &mut Core, e: u16, v: u32) -> (u64, i32) {
        let key = c.random();
        let bonus = if self.team && e < 10 && e >= 5 {
            0.1
        } else {
            0.0
        };
        (
            key,
            i32::from(unit(key) < self.versions[v as usize].chance + bonus),
        )
    }
    fn record(&mut self, c: &mut Core, e: u16, v: u32, r: (u64, i32), sub: Option<u16>) {
        let live = self.snapshot(c);
        let f = catalog::TX[e as usize];
        c.record(Hit {
            skill: f.0,
            target: if f.7 == 3 { NONE } else { 0 },
            level: f.2,
            outcome: r.1,
            key: r.0,
            effect: e,
            profile: (self.versions[v as usize].profile & !104)
                | (self.versions[live as usize].profile & 104),
            sub: sub.unwrap_or(f.1),
            frozen: v,
            live,
            ..Hit::default()
        });
    }
    fn hit(&mut self, c: &mut Core, e: u16) -> i32 {
        let v = self.snapshot(c);
        let r = self.roll(c, e, v);
        self.record(c, e, v, r, None);
        r.1
    }
    fn dot_hit(&mut self, c: &mut Core, e: u16, v: u32, sub: Option<u16>) {
        let r = self.roll(c, e, v);
        self.record(c, e, v, r, sub);
    }
    fn aura(&mut self, c: &mut Core, s: u16, d: i32) {
        c.buff(s, false, c.now + d, 0, 1);
        self.profile |= match s {
            SET => 128,
            LIEYUN => 16,
            SUIXING_BUFF => 2 | if self.team { 8 } else { 0 },
            _ => unreachable!(),
        };
        c.arm(EXPIRE, s, c.now + d);
    }
    fn update_lieyun(&mut self, c: &mut Core) {
        if c.stacks(DIEREN) < 4 {
            if c.stacks(HIDDEN) > 0 {
                c.buff(HIDDEN, true, 0, 0, 0);
                c.cancel(TICK, HIDDEN);
                c.cancel(EXPIRE, HIDDEN);
            }
            return;
        }
        let active = c.stacks(HIDDEN) > 0;
        let next = if active {
            c.buffs[HIDDEN as usize].next
        } else {
            c.now + 8
        };
        if !active {
            self.aura(c, LIEYUN, 16);
        }
        c.buff(HIDDEN, true, c.now + 384, next, 1);
        c.arm(TICK, HIDDEN, next);
        c.arm(EXPIRE, HIDDEN, c.now + 384);
    }
    fn add_dot(&mut self, c: &mut Core, s: u16, add: i32) {
        let b = c.buffs[s as usize];
        let next = if b.n == 0 { c.now + 48 } else { b.next };
        let end = next + 48 * if s == DIEREN { 7 } else { 9 };
        c.buff(
            s,
            true,
            end,
            next,
            (b.n + add).min(if s == DIEREN { 7 } else { 3 }),
        );
        if s == DIEREN {
            self.update_lieyun(c);
            self.dot = self.snapshot(c);
        } else {
            self.wan = self.snapshot(c);
        }
        c.arm(TICK, s, next);
        c.arm(EXPIRE, s, end);
    }
    fn equipment(&mut self, c: &mut Core) {
        if unit(c.random()) < 31.0 / 1024.0 && c.now >= self.weapon_ready {
            self.weapon_ready = c.now + 480;
            c.buff(WEAPON, false, c.now + 96, 0, 1);
            c.cd(BAHUANG, c.now);
            c.cancel(COOLDOWN, BAHUANG);
            c.arm(EXPIRE, WEAPON, c.now + 96);
        }
    }
    fn team_buff(&mut self, c: &mut Core, s: u16, d: i32, n: i32) {
        if c.stacks(s) != n {
            self.team_version += 1;
        }
        let end = if n == 0 { 0 } else { c.now + d };
        let next = if s == DRUM && n != 0 { c.now + 480 } else { 0 };
        c.buff(s, s == POFENG, end, next, n);
        if s == DRUM {
            if next > 0 {
                c.arm(TICK, s, next);
            } else {
                c.cancel(TICK, s);
            }
        }
        if n != 0 {
            c.arm(EXPIRE, s, end);
        } else {
            c.cancel(EXPIRE, s);
        }
    }
    fn publish_fields(&self, c: &mut Core, skill: u16) {
        let mut n = 0;
        let mut end = i32::MAX;
        let mut next = i32::MAX;
        for f in &self.fields {
            if f.end != 0 && f.skill == skill {
                n += 1;
                end = end.min(f.end);
                next = next.min(f.next);
            }
        }
        c.buff(
            skill,
            false,
            if n == 0 { 0 } else { end },
            if n == 0 { 0 } else { next },
            n,
        );
    }
    fn oldest(&self) -> Option<usize> {
        (0..3)
            .filter(|&i| self.fields[i].end != 0)
            .min_by_key(|&i| self.fields[i].order)
    }
    fn add_field(&mut self, c: &mut Core, k: u16) {
        let slot = if let Some(i) = self.fields.iter().position(|f| f.end == 0) {
            i
        } else {
            let i = self.oldest().unwrap();
            let old = self.fields[i].skill;
            self.fields[i] = FieldState::default();
            c.cancel(FIELD, i as u16);
            self.publish_fields(c, old);
            i
        };
        self.fields[slot] = FieldState {
            skill: k,
            end: c.now + 384,
            next: c.now + 48,
            order: self.field_order,
        };
        self.field_order += 1;
        c.arm(FIELD, slot as u16, c.now + 48);
        self.publish_fields(c, k);
        if k == SUIXING {
            self.aura(c, SUIXING_BUFF, 64);
        }
    }
    fn cast(&mut self, c: &mut Core, k: u16) {
        if k <= SANHUAN || matches!(k, SUIXING | SHENGTAI | RENJIAN) {
            c.gcd(0, c.now + 24);
            c.arm(GCD, NONE, c.now + 24);
        }
        match k {
            SUIXING | SHENGTAI => {
                self.preparing = true;
                c.change(5, k, false, c.now + 16, 1, 0);
                c.cd(k, i32::MAX);
                c.arm(PREPARE, k, c.now + 16);
                return;
            }
            RENJIAN => {
                for skill in [RENJIAN, 13] {
                    c.cd(skill, c.now + 320);
                    c.arm(COOLDOWN, skill, c.now + 320);
                }
                let mut count = 0;
                while let Some(i) = self.oldest() {
                    let old = self.fields[i].skill;
                    self.fields[i] = FieldState::default();
                    c.cancel(FIELD, i as u16);
                    self.publish_fields(c, old);
                    count += 1;
                    let v = self.snapshot(c);
                    let r = self.roll(c, 105, v);
                    if self.team {
                        self.qi(c, self.qi + 2);
                    }
                    self.record(c, 105, v, r, None);
                    if self.team {
                        self.equipment(c);
                    }
                }
                if self.team {
                    let n = (c.stacks(XUANMEN) + count).min(3);
                    c.buff(XUANMEN, false, c.now + 640, 0, n);
                    self.profile = (self.profile & !96) | (n as u16 * 32);
                    c.arm(EXPIRE, XUANMEN, c.now + 640);
                    self.aura(c, SET, 96);
                }
                return;
            }
            ZIQI => {
                if c.energy[k as usize] == 2 {
                    self.recharge = c.now + 1200;
                    c.arm(COOLDOWN, k, self.recharge);
                }
                c.charge(k, c.energy[k as usize] - 1);
                c.cd(
                    k,
                    if c.energy[k as usize] > 0 {
                        0
                    } else {
                        self.recharge
                    },
                );
                self.qi(c, 10);
                c.buff(PURPLE, false, c.now + 160, c.now + 16, 1);
                self.profile |= 1;
                c.arm(TICK, PURPLE, c.now + 16);
                c.arm(EXPIRE, PURPLE, c.now + 160);
                let v = self.snapshot(c);
                self.record(c, 99, v, (0, 0), None);
                return;
            }
            WUWO => {
                let n = self.qi;
                self.qi(c, 0);
                let crit = self.hit(c, (n - 1) as u16);
                if n >= 6 {
                    self.hit(c, (33 + (n - 6) / 2) as u16);
                }
                if self.team {
                    self.add_dot(c, DIEREN, 1 + crit);
                    self.qi(c, self.qi + 2);
                }
            }
            BAHUANG => {
                let free = c.stacks(WEAPON) > 0;
                if free {
                    c.cd(k, c.now);
                    c.cancel(COOLDOWN, k);
                } else {
                    c.cd(k, c.now + 192);
                    c.arm(COOLDOWN, k, c.now + 192);
                }
                self.qi(c, self.qi + 2);
                self.hit(c, 20);
                self.hit(c, if free { 115 } else { 21 });
                if self.team {
                    self.qi(c, self.qi + 2);
                }
            }
            SANHUAN => {
                c.cd(k, c.now + 32);
                c.arm(COOLDOWN, k, c.now + 32);
                self.qi(c, self.qi + 2);
                for _ in 0..3 {
                    self.hit(c, 31);
                }
                if self.team {
                    self.qi(c, self.qi + 2);
                }
            }
            SANCHAI => {
                c.cd(k, c.now + 22);
                c.arm(COOLDOWN, k, c.now + 22);
                let v = self.snapshot(c);
                let r = self.roll(c, 32, v);
                if self.team {
                    self.qi(c, self.qi + 2);
                }
                self.record(c, 32, v, r, None);
            }
            _ => unreachable!(),
        }
        if self.team {
            self.equipment(c);
            if k == BAHUANG {
                if c.stacks(WEAPON) > 0 {
                    self.add_dot(c, WANXIANG, 1);
                }
                if unit(c.random()) < 307.0 / 1024.0 {
                    self.hit(c, 116);
                }
            }
            self.aura(c, SET, 96);
        }
    }
}
impl Rules for TaiXu {
    const TIMERS: &'static [Timer] = catalog::TX_TIMERS;
    const SKILLS: usize = 19;
    fn new(team: bool) -> Self {
        Self {
            fields: [FieldState::default(); 3],
            field_order: 0,
            preparing: false,
            team,
            qi: 10,
            recharge: 0,
            weapon_ready: 480,
            profile: 0,
            team_version: 0,
            cursor: 0,
            dot: 0,
            wan: 0,
            versions: Vec::with_capacity(4096),
            generation: 0,
            cache: [Cache::default(); 1024],
        }
    }
    fn reset(&mut self, c: &mut Core) {
        self.fields.fill(FieldState::default());
        self.field_order = 0;
        self.preparing = false;
        self.qi = 10;
        self.recharge = 0;
        self.weapon_ready = 480;
        self.profile = 0;
        self.team_version = 0;
        self.cursor = 0;
        self.versions.clear();
        self.generation += 1;
        c.energy[ZIQI as usize] = 2;
        if self.team {
            c.arm(TEAM, NONE, 0);
        }
        let phase = mix((c.rng ^ 0x6a09e667f3bcc909).wrapping_add(STEP)) & 15;
        c.arm(REGEN, NONE, phase as i32);
    }
    fn ready(&self, c: &Core, k: u16) -> i32 {
        let cd = c.ready[k as usize];
        if k <= SANHUAN || matches!(k, SUIXING | SHENGTAI | RENJIAN) {
            cd.max(c.gcd[0])
        } else {
            cd
        }
    }
    fn qi(&self) -> i32 {
        self.qi
    }
    fn busy(&self) -> bool {
        self.preparing
    }
    fn available(&self, c: &Core, k: u16) -> bool {
        if self.preparing {
            return false;
        }
        if k == BAHUANG && c.stacks(WEAPON) > 0 {
            c.gcd[0] <= c.now
        } else {
            self.ready(c, k) <= c.now
        }
    }
    fn perform(&mut self, c: &mut Core, k: u16) -> bool {
        if k == ZIQI && c.energy[k as usize] == 0 || k == WUWO && self.qi == 0 {
            return false;
        }
        if !matches!(k, SANCHAI | SUIXING | SHENGTAI) {
            c.last_skill = k;
        }
        self.cast(c, k);
        true
    }
    fn timer(&mut self, c: &mut Core, t: Timer) {
        let s = t.1;
        match (t.0, s) {
            (PREPARE, k) => {
                self.preparing = false;
                c.change(5, k, false, 0, 0, 0);
                c.last_skill = k;
                let at = c.now + if k == SHENGTAI && self.team { 400 } else { 160 };
                c.cd(k, at);
                c.arm(COOLDOWN, k, at);
                self.qi(c, self.qi + 2);
                self.add_field(c, k);
                let v = self.snapshot(c);
                self.record(c, 100 + k - SUIXING, v, (0, 0), None);
            }
            (FIELD, i) => {
                let slot = i as usize;
                let mut f = self.fields[slot];
                if f.next == c.now {
                    if f.skill == SUIXING {
                        self.aura(c, SUIXING_BUFF, 64);
                    }
                    f.next += 48;
                }
                if f.end == c.now {
                    self.fields[slot] = FieldState::default();
                } else {
                    self.fields[slot] = f;
                    c.arm(FIELD, i, f.next.min(f.end));
                }
                self.publish_fields(c, f.skill);
            }

            (TEAM, _) => {
                let (_, slot, d, n) = SCHEDULE[self.cursor];
                self.team_buff(c, slot, d, n);
                self.cursor += 1;
                if self.cursor < SCHEDULE.len() {
                    c.arm(TEAM, NONE, SCHEDULE[self.cursor].0);
                }
            }
            (TICK, DRUM) => {
                let b = c.buffs[s as usize];
                self.team_version += 1;
                c.buff(s, false, b.end, 0, b.n / 2);
            }
            (EXPIRE, 23..) => self.team_buff(c, s, 0, 0),
            (REGEN, _) => {
                if self.qi < 10 {
                    self.qi(c, self.qi + 1);
                }
                c.arm(REGEN, NONE, c.now + 16);
            }
            (COOLDOWN, ZIQI) => {
                let n = c.energy[s as usize] + 1;
                c.charge(s, n);
                if n < 2 {
                    self.recharge = c.now + 1200;
                    c.arm(COOLDOWN, s, self.recharge);
                } else {
                    self.recharge = 0;
                }
                c.cd(s, 0);
            }
            (TICK, HIDDEN) => {
                self.aura(c, LIEYUN, 16);
                c.buff(s, true, c.buffs[s as usize].end, c.now + 8, 1);
                c.arm(TICK, s, c.now + 8);
            }
            (TICK, DIEREN | WANXIANG) => {
                let b = c.buffs[s as usize];
                if s == DIEREN {
                    self.dot_hit(c, 37 + (b.n as u16 - 1) * 9, self.dot, None);
                } else {
                    self.dot_hit(c, 116 + b.n as u16, self.wan, Some(b.n as u16));
                }
                c.buff(s, true, b.end, c.now + 48, b.n);
                c.arm(TICK, s, c.now + 48);
            }
            (TICK, PURPLE) => {
                self.qi(c, self.qi + 2);
                c.buff(s, false, c.buffs[s as usize].end, c.now + 16, 1);
                c.arm(TICK, s, c.now + 16);
            }
            (EXPIRE, WEAPON) => {
                c.buff(s, false, 0, 0, 0);
                c.cd(BAHUANG, c.now);
                c.cancel(COOLDOWN, BAHUANG);
            }
            (EXPIRE, DIEREN) => {
                self.update_lieyun(c);
                c.buff(s, true, 0, 0, 0);
                c.cancel(TICK, s);
                c.cancel(EXPIRE, s);
            }
            (EXPIRE, _) => {
                c.buff(s, t.2, 0, 0, 0);
                match s {
                    PURPLE => {
                        self.profile &= !1;
                        c.cancel(TICK, s);
                    }
                    HIDDEN | WANXIANG => c.cancel(TICK, s),
                    SUIXING_BUFF => self.profile &= !10,
                    XUANMEN => self.profile &= !96,
                    SET => self.profile &= !128,
                    LIEYUN => self.profile &= !16,
                    _ => {}
                }
            }
            _ => {}
        }
    }
    fn reduce(&mut self, h: &Hit) -> i64 {
        let roll = 2 * h.effect as u32 + h.outcome as u32;
        let index =
            (h.frozen.wrapping_mul(0x9e3779b1) ^ h.live.wrapping_mul(0x85ebca6b) ^ roll * 17)
                & 1023;
        let cached = &mut self.cache[index as usize];
        if (cached.generation, cached.frozen, cached.live, cached.roll)
            == (self.generation, h.frozen, h.live, roll)
        {
            return cached.damage;
        }
        let f = catalog::TX[h.effect as usize];
        let a = self.versions[h.frozen as usize];
        let live = self.versions[h.live as usize];
        let bonus = if f.0 == SANCHAI {
            205
        } else if self.team && (f.0 == WUWO || f.0 == BAHUANG) {
            51
        } else {
            0
        };
        let raw =
            f.base(a.attack, 2000, 10.0) * (1024 + bonus) / 1024 * f.6 * live.mitigation / 1024;
        let power = a.power
            + if self.team && f.0 == WUWO && f.2 >= 5 {
                307
            } else {
                0
            };
        let damage = finish(raw, h.outcome == 1, power, a.strain);
        *cached = Cache {
            generation: self.generation,
            frozen: h.frozen,
            live: h.live,
            roll,
            damage,
        };
        damage
    }
}
