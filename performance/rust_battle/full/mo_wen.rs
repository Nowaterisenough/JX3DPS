use super::*;
use std::sync::OnceLock;

const BIANGONG: u16 = 2;
const SHANG: u16 = 3;
const JUE: u16 = 4;
const ZHI: u16 = 5;
const BIANZHI: u16 = 6;
const YU: u16 = 7;
const GAOSHAN: u16 = 8;
const SHUYING: u16 = 10;
const SHANG_DOT: u16 = 0;
const JUE_DOT: u16 = 1;
const LIUZHAO: u16 = 7;
const QUFENG: u16 = 8;
const GAOSHAN_BUFF: u16 = 9;
const YANGCHUN_BUFF: u16 = 10;
const YINGZI: u16 = 15;
const SET: u16 = 16;
const WEAPON: u16 = 17;
const SHENBING: u16 = 18;
const YOUREN: u16 = 19;
const JINGMIAO: u16 = 20;
const BELT: u16 = 23;
const WATER: u16 = 24;
const E_SHANG: u16 = 2;
const E_JUE: u16 = 3;
const E_BIANZHI: u16 = 5;
const E_YU: u16 = 6;
const E_SHADOW: u16 = 16;
const E_LIUZHAO: u16 = 18;
const E_LIUZHAO_SURPLUS: u16 = 19;
const E_BIANGONG_CW: u16 = 21;
const E_YU_CW: u16 = 22;
const E_SHOES: u16 = 36;
const E_WRIST: u16 = 37;
#[derive(Clone, Copy)]
struct Damage {
    before: i64,
    power: i64,
    amount: [i64; 2],
}
pub(crate) struct Tables {
    rows: Vec<Damage>,
    water_count: usize,
    strain: i64,
}
static DEFAULT: OnceLock<Tables> = OnceLock::new();
static EQUIPPED: OnceLock<Tables> = OnceLock::new();
pub fn prepare(equipped: bool) -> &'static Tables {
    let cell = if equipped { &EQUIPPED } else { &DEFAULT };
    cell.get_or_init(|| {
        let strain = (10000.0 * 1024.0 / (9.189 * 8250.0)) as i64 + 1024;
        let wc = if equipped { 11 } else { 1 };
        let belts = if equipped { 3 } else { 1 };
        let mut rows = Vec::with_capacity(wc * belts * 256 * catalog::MW.len());
        for belt in 0..belts {
            for water in 0..wc {
                for profile in 0..256 {
                    for (i, &f) in catalog::MW.iter().enumerate() {
                        let atk = (10000 + 88 * water as i64)
                            * (1024 + if profile & 8 != 0 { 102 } else { 0 })
                            / 1024
                            + 2500;
                        let mut bonus = 51 * (profile & 7)
                            + match belt {
                                1 => 10,
                                2 => 51,
                                _ => 0,
                            };
                        let mut power = (15000.0 * 1024.0 / (3.335 * 8250.0)) as i64
                            + if profile & 128 != 0 { 41 } else { 0 };
                        if (i <= 6 || i == 14 || i == 15) && profile & 16 != 0 {
                            bonus += 205;
                        }
                        if equipped {
                            if i <= 1 || matches!(i, 16 | 20 | 21) {
                                power += 102;
                            }
                            if i == 4 {
                                bonus += 102;
                            } else if i == 5 {
                                bonus += 152;
                            }
                            if matches!(i, 0 | 1 | 4 | 5) {
                                bonus += 153;
                            }
                        }
                        if i == 8 {
                            bonus += 410;
                        }
                        let ignore = match i {
                            0 | 4 | 20 => 307 * ((profile & 96) / 32),
                            6 | 22 if equipped => 614,
                            18 => 922,
                            _ => 0,
                        };
                        let before = f.base(atk, 0, 12.0) * (1024 + bonus) / 1024
                            * mitigation(25000, 30000 - 30000 * ignore / 1024)
                            / 1024;
                        rows.push(Damage {
                            before,
                            power,
                            amount: [
                                finish(before, false, power, strain),
                                finish(before, true, power, strain),
                            ],
                        });
                    }
                }
            }
        }
        Tables {
            rows,
            water_count: wc,
            strain,
        }
    })
}
#[derive(Clone, Copy, Default)]
struct Snapshot {
    profile: u16,
    extra: i32,
}
#[derive(Clone, Copy, Default)]
struct Shadow {
    end: i32,
    next: i32,
    order: u32,
    snap: Snapshot,
}
pub struct MoWen {
    equipped: bool,
    data: &'static Tables,
    snap: Snapshot,
    shadows: [Shadow; 6],
    shadow_count: i32,
    order: u32,
    recharge: [i32; 11],
    prepared: bool,
    channel: i32,
    weapon_ready: i32,
    shoes_ready: i32,
    wrist_ready: i32,
    belt_ready: i32,
    dot_snap: [Snapshot; 25],
    dot_interval: [i32; 25],
    haste: f64,
}
impl MoWen {
    fn duration(&self, base: i32) -> i32 {
        (base as f64 * self.haste) as i32
    }
    fn aura(&mut self, c: &mut Core, s: u16, d: i32, n: i32) {
        let end = if n == 0 {
            0
        } else if d == i32::MAX {
            d
        } else {
            c.now + d
        };
        c.buff(s, false, end, 0, n);
        match s {
            GAOSHAN_BUFF => self.haste = if n != 0 { 1024.0 / 1229.0 } else { 1.0 },
            JINGMIAO => self.snap.extra = if n != 0 { 205 } else { 0 },
            SET | YOUREN => {
                let mask = if s == SET { 128 } else { 256 };
                self.snap.profile = (self.snap.profile & !mask) | if n != 0 { mask } else { 0 };
            }
            WATER => self.snap.profile = (self.snap.profile & !(15 * 512)) | n as u16 * 512,
            BELT if n == 0 => self.snap.profile &= !(3 * 8192),
            _ => {}
        }
        if d != i32::MAX && n != 0 {
            c.arm(EXPIRE, s, end);
        } else {
            c.cancel(EXPIRE, s);
        }
    }
    fn hit(&mut self, c: &mut Core, e: u16, snapshot: Snapshot, you_ren: bool) {
        let k = c.random();
        let p = snapshot.profile;
        let bonus = if p & 128 != 0 { 400 } else { 0 }
            + if p & 256 != 0 { 500 } else { 0 }
            + if self.equipped && matches!(e, 0 | 1 | E_SHADOW) {
                1000
            } else {
                0
            };
        let crit = unit(k) < 20000.0 / (9.530 * 8250.0) + bonus as f64 / 10000.0;
        if self.equipped && crit {
            if (e <= 9 || matches!(e, 20..=22)) && c.now >= self.shoes_ready {
                self.shoes_ready = c.now + 160;
                self.hit(c, E_SHOES, self.snap, false);
            }
            if you_ren {
                self.aura(c, YOUREN, 80, 1);
            }
        }
        let f = catalog::MW[e as usize];
        c.record(Hit {
            skill: f.0,
            level: f.2,
            sub: f.1,
            effect: e,
            profile: p,
            base: snapshot.extra as i64,
            key: k,
            outcome: i32::from(crit),
            ..Hit::default()
        });
    }
    fn zero(&self, c: &mut Core, e: u16) {
        c.record(Hit {
            skill: catalog::MW[e as usize].0,
            target: NONE,
            effect: e,
            ..Hit::default()
        });
    }
    fn gcd(&self, c: &mut Core, g: u16, d: i32) {
        c.gcd(g, c.now + d);
        c.arm(GCD, g, c.now + d);
    }
    fn wrist(&mut self, c: &mut Core) {
        if self.equipped && c.now >= self.wrist_ready && c.chance(10, 100) {
            self.wrist_ready = c.now + 160;
            self.hit(c, E_WRIST, self.snap, false);
        }
    }
    fn enchants(&mut self, c: &mut Core) {
        self.wrist(c);
        if self.equipped && c.now >= self.belt_ready && c.chance(20, 100) {
            self.belt_ready = c.now + 480;
            let bonus = if c.chance(70, 100) { 2 } else { 1 };
            self.snap.profile = (self.snap.profile & !(3 * 8192)) | bonus * 8192;
            self.aura(c, BELT, 128, 1);
        }
    }
    fn water(&mut self, c: &mut Core) {
        if self.equipped {
            self.aura(c, WATER, 96, (c.stacks(WATER) + 1).min(10));
        }
    }
    fn equipment(&mut self, c: &mut Core) {
        if !self.equipped {
            return;
        }
        if c.chance(10, 100) {
            self.aura(c, SET, 96, 1);
        }
        if c.chance(25, 1024) && c.now >= self.weapon_ready {
            self.weapon_ready = c.now + 480;
            self.aura(c, WEAPON, 96, 1);
            for k in [YU, ZHI] {
                self.recharge[k as usize] = c.now;
                c.arm(COOLDOWN, k, c.now);
                if c.energy[k as usize] == 0 {
                    c.cd(k, c.now);
                    if k == ZHI {
                        c.cd(BIANZHI, c.now);
                    }
                }
            }
        }
    }
    fn recharge_time(k: u16) -> i32 {
        match k {
            YU => 96,
            SHUYING => 320,
            ZHI => 128,
            _ => unreachable!(),
        }
    }
    fn publish(&mut self, c: &mut Core, k: u16, n: i32) {
        let at = self.recharge[k as usize];
        let ready = if n != 0 { 0 } else { at };
        c.charge(k, n);
        c.cd(k, ready);
        if k == ZHI {
            c.charge(BIANZHI, n);
            c.cd(BIANZHI, ready);
        }
        if n < 3 {
            c.arm(COOLDOWN, k, at);
        } else {
            self.recharge[k as usize] = 0;
            c.cancel(COOLDOWN, k);
        }
    }
    fn consume(&mut self, c: &mut Core, k: u16) {
        let n = c.energy[k as usize];
        if n == 3 {
            self.recharge[k as usize] = c.now + Self::recharge_time(k);
        }
        self.publish(c, k, n - 1);
    }
    fn shorten(&mut self, c: &mut Core, k: u16) {
        let mut n = c.energy[k as usize];
        if n == 3 {
            return;
        }
        let at = &mut self.recharge[k as usize];
        *at -= 80;
        if *at <= c.now {
            n += 1;
            *at += Self::recharge_time(k);
        }
        self.publish(c, k, n);
    }
    fn surplus(&mut self, c: &mut Core) {
        let n = c.stacks(LIUZHAO);
        if self.equipped && n > 0 {
            for _ in 0..n {
                self.hit(c, E_LIUZHAO_SURPLUS, self.snap, false);
            }
            self.aura(c, LIUZHAO, 0, 0);
        }
        for e in [10, 11, 12] {
            self.hit(c, e, self.snap, false);
        }
    }
    fn qufeng(&mut self, c: &mut Core, add: i32) {
        let n = c.stacks(QUFENG) + add;
        self.aura(c, QUFENG, i32::MAX, if n >= 9 { 0 } else { n });
        if n == 9 {
            self.surplus(c);
        }
    }
    fn add_dot(&mut self, c: &mut Core, s: u16) {
        let b = c.buffs[s as usize];
        let dt = self.duration(48);
        let next = if b.n != 0 { b.next } else { c.now + dt };
        let end = next + self.duration(48 * if s == SHENBING { 9 } else { 5 });
        self.dot_snap[s as usize] = self.snap;
        self.dot_interval[s as usize] = dt;
        c.buff(
            s,
            true,
            end,
            next,
            if s == SHENBING { (b.n + 1).min(3) } else { 1 },
        );
        c.arm(TICK, s, next);
        c.arm(EXPIRE, s, end);
    }
    fn publish_shadows(&self, c: &mut Core) {
        let mut end = i32::MAX;
        let mut tick = i32::MAX;
        for shadow in &self.shadows {
            if shadow.end > 0 {
                end = end.min(shadow.end);
                tick = tick.min(shadow.next);
            }
        }
        if self.shadow_count == 0 {
            end = 0;
            tick = 0;
        }
        c.buff(YINGZI, false, end, tick, self.shadow_count);
    }
    fn remove_shadow(&mut self, c: &mut Core, i: usize) {
        self.shadows[i] = Shadow::default();
        self.shadow_count -= 1;
        c.cancel(FIELD, i as u16);
        self.publish_shadows(c);
    }
    fn add_shadow(&mut self, c: &mut Core) {
        let snap = self.snap;
        if self.shadow_count == 6 {
            let i = (0..6).min_by_key(|&j| self.shadows[j].order).unwrap();
            self.remove_shadow(c, i);
        }
        let i = self.shadows.iter().position(|s| s.end == 0).unwrap();
        self.shadow_count += 1;
        self.shadows[i] = Shadow {
            end: c.now + 400,
            next: c.now + self.duration(32),
            order: self.order,
            snap,
        };
        self.order += 1;
        c.arm(FIELD, i as u16, self.shadows[i].next);
        self.publish_shadows(c);
    }
    fn gong(&mut self, c: &mut Core, cw: bool) {
        if !cw {
            self.qufeng(c, 4);
        }
        self.enchants(c);
        self.equipment(c);
        if cw {
            if c.stacks(WEAPON) > 0 {
                self.add_dot(c, SHENBING);
            }
            self.water(c);
            self.surplus(c);
            self.aura(c, QUFENG, i32::MAX, 0);
        } else {
            self.water(c);
        }
        self.hit(c, if cw { E_BIANGONG_CW } else { 1 }, self.snap, !cw);
        self.aura(c, YANGCHUN_BUFF, 0, 0);
    }
    fn cast(&mut self, c: &mut Core, k: u16) {
        if k <= YU {
            self.gcd(c, 0, self.duration(24));
        }
        match k {
            SHANG | JUE => {
                self.enchants(c);
                self.equipment(c);
                self.water(c);
                self.hit(c, if k == SHANG { E_SHANG } else { E_JUE }, self.snap, true);
                self.qufeng(c, if k == SHANG { 3 } else { 2 });
                self.add_dot(c, if k == SHANG { SHANG_DOT } else { JUE_DOT });
            }
            SHUYING => {
                self.gcd(c, 4, self.duration(24));
                self.consume(c, k);
                self.zero(c, 31);
                self.wrist(c);
                self.add_shadow(c);
                if c.chance(if self.equipped { 100 } else { 20 }, 100) {
                    self.add_shadow(c);
                }
            }
            GAOSHAN => {
                self.zero(c, 29);
                self.gcd(c, 2, 800);
                self.wrist(c);
                self.aura(c, GAOSHAN_BUFF, 192, 1);
            }
            YU => {
                self.consume(c, YU);
                self.qufeng(c, 5);
                self.enchants(c);
                self.equipment(c);
                self.water(c);
                if self.equipped {
                    if c.chance(307, 1024) {
                        self.hit(c, E_YU_CW, self.snap, true);
                    }
                    self.shorten(c, YU);
                    self.shorten(c, ZHI);
                }
                self.hit(c, E_YU, self.snap, false);
            }
            BIANZHI => {
                if self.equipped {
                    self.consume(c, ZHI);
                }
                self.channel = 1;
                let dt = self.duration(16);
                c.change(6, k, false, c.now + 3 * dt, 1, 0);
                c.arm(CHANNEL, NONE, c.now + dt);
                self.qufeng(c, 6);
            }
            BIANGONG => {
                if c.stacks(WEAPON) > 0 {
                    self.gong(c, true);
                } else {
                    self.prepared = true;
                    let at = c.now + self.duration(28);
                    c.change(5, k, false, at, 1, 0);
                    c.arm(PREPARE, k, at);
                }
            }
            _ => unreachable!(),
        }
    }
}
impl Rules for MoWen {
    const TIMERS: &'static [Timer] = catalog::MW_TIMERS;
    const SKILLS: usize = 31;
    fn new(equipped: bool) -> Self {
        Self {
            equipped,
            data: prepare(equipped),
            snap: Snapshot::default(),
            shadows: [Shadow::default(); 6],
            shadow_count: 0,
            order: 0,
            recharge: [0; 11],
            prepared: false,
            channel: 0,
            weapon_ready: 480,
            shoes_ready: 0,
            wrist_ready: 0,
            belt_ready: 0,
            dot_snap: [Snapshot::default(); 25],
            dot_interval: [0; 25],
            haste: 1.0,
        }
    }
    fn reset(&mut self, c: &mut Core) {
        self.snap = Snapshot::default();
        self.shadows.fill(Shadow::default());
        self.shadow_count = 0;
        self.order = 0;
        self.recharge.fill(0);
        self.prepared = false;
        self.channel = 0;
        self.haste = 1.0;
        self.weapon_ready = 480;
        self.shoes_ready = 0;
        self.wrist_ready = 0;
        self.belt_ready = 0;
        c.energy[YU as usize] = 3;
        c.energy[SHUYING as usize] = 3;
        if self.equipped {
            c.energy[ZHI as usize] = 3;
            c.energy[BIANZHI as usize] = 3;
            let delay = (c.random() % 3) as i32;
            c.arm(TICK, JINGMIAO, delay);
        }
    }
    fn ready(&self, c: &Core, k: u16) -> i32 {
        let g = match k {
            SHUYING => 4,
            GAOSHAN => 2,
            _ => 0,
        };
        c.ready[k as usize].max(c.gcd[g])
    }
    fn busy(&self) -> bool {
        self.prepared || self.channel != 0
    }
    fn interrupt(&mut self, c: &mut Core) {
        if self.channel != 0 {
            c.change(6, BIANZHI, false, 0, 0, 0);
            c.cancel(CHANNEL, NONE);
            self.channel = 0;
            self.aura(c, YANGCHUN_BUFF, 0, 0);
        }
    }
    fn perform(&mut self, c: &mut Core, k: u16) -> bool {
        if ((matches!(k, YU | SHUYING) || k == BIANZHI && self.equipped)
            && c.energy[k as usize] == 0)
            || k == SHUYING && self.shadow_count == 6
        {
            return false;
        }
        c.last_skill = k;
        self.cast(c, k);
        true
    }
    fn timer(&mut self, c: &mut Core, t: Timer) {
        let s = t.1;
        match t.0 {
            COOLDOWN => {
                let n = (c.energy[s as usize] + 1).min(3);
                self.recharge[s as usize] = c.now + Self::recharge_time(s);
                self.publish(c, s, n);
            }
            PREPARE => {
                self.prepared = false;
                c.change(5, s, false, 0, 0, 0);
                self.gong(c, false);
            }
            CHANNEL => {
                self.equipment(c);
                self.water(c);
                if self.equipped {
                    let n = i32::from(c.stacks(SHANG_DOT) > 0) + i32::from(c.stacks(JUE_DOT) > 0);
                    for _ in 0..n {
                        self.hit(c, E_LIUZHAO, self.snap, false);
                    }
                    if n > 0 {
                        self.aura(c, LIUZHAO, 400, (c.stacks(LIUZHAO) + n).min(32));
                    }
                }
                self.enchants(c);
                self.hit(c, E_BIANZHI, self.snap, true);
                if self.channel < 3 {
                    let dt = self.duration(16);
                    c.change(6, BIANZHI, false, c.now + (3 - self.channel) * dt, 1, 0);
                    c.arm(CHANNEL, NONE, c.now + dt);
                    self.channel += 1;
                } else {
                    self.channel = 0;
                    c.change(6, BIANZHI, false, 0, 0, 0);
                }
            }
            TICK if s == JINGMIAO => {
                self.aura(c, s, 80, 1);
                let offset = (c.random() % 7) as i32;
                c.arm(TICK, s, c.now + 320 + offset);
            }
            TICK => {
                let b = c.buffs[s as usize];
                let effect = match s {
                    SHANG_DOT => 14,
                    JUE_DOT => 15,
                    SHENBING => 22 + b.n as u16,
                    _ => unreachable!(),
                };
                self.hit(c, effect, self.dot_snap[s as usize], false);
                let next = c.now + self.dot_interval[s as usize];
                c.buff(s, true, b.end, next, b.n);
                if next <= b.end {
                    c.arm(TICK, s, next);
                }
            }
            EXPIRE if matches!(s, SHANG_DOT | JUE_DOT | SHENBING) => {
                c.buff(s, true, 0, 0, 0);
                c.cancel(TICK, s);
            }
            EXPIRE => self.aura(c, s, 0, 0),
            FIELD => {
                let i = s as usize;
                let mut shadow = self.shadows[i];
                if c.now == shadow.next {
                    self.hit(c, E_SHADOW, shadow.snap, false);
                    shadow.next = c.now + self.duration(32);
                    self.shadows[i] = shadow;
                }
                if c.now == shadow.end {
                    self.remove_shadow(c, i);
                } else {
                    c.arm(FIELD, s, shadow.end.min(shadow.next));
                    self.publish_shadows(c);
                }
            }
            GCD => {}
            _ => unreachable!(),
        }
    }
    fn reduce(&mut self, h: &Hit) -> i64 {
        let p = h.profile as usize;
        let idx = (p & 255) + 256 * ((p >> 9 & 15) + self.data.water_count * (p >> 13 & 3));
        let d = self.data.rows[idx * catalog::MW.len() + h.effect as usize];
        if h.outcome == 1 && h.base != 0 {
            finish(d.before, true, d.power + h.base, self.data.strain)
        } else {
            d.amount[h.outcome as usize]
        }
    }
}
