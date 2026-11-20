use super::*;
pub const TX_MACRO: &str = "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [qidian>=8] wuwu\n/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";
pub const MW_MACRO: &str = "/cast [tnobuff:shang] shang\n/cast [tnobuff:jue] jue\n/cast shuying\n/cast gaoshan\n/cast [buff:qufeng=4] yu\n/cast bianzhi\n/cast biangong";
#[derive(Clone, Copy)]
enum Value {
    Qi,
    Fixed(f64),
    Stacks(u16),
    Duration(u16),
    Cooldown(u16),
    Energy(u16),
    Last(u16),
}
#[derive(Clone, Copy)]
enum Compare {
    Lt,
    Le,
    Eq,
    Ne,
    Gt,
    Ge,
}
#[derive(Clone, Copy)]
struct Condition {
    value: Value,
    cmp: Compare,
    rhs: f64,
    yes: usize,
    no: usize,
}
struct Line {
    skill: u16,
    command: u8,
    start: usize,
    end: usize,
}
pub struct Program {
    lines: Vec<Line>,
    conditions: Vec<Condition>,
    time: Vec<Condition>,
    force: bool,
}
const YES: usize = usize::MAX;
const NO: usize = YES - 1;
impl Program {
    pub fn len(&self) -> usize {
        self.lines.len()
    }
    pub fn compile(tx: bool, text: &str) -> Result<Self, String> {
        let mut p = Self {
            lines: vec![],
            conditions: vec![],
            time: vec![],
            force: false,
        };
        let skills: &[(&str, u16)] = if tx {
            &[
                ("ziqi", 4),
                ("wuwu", 0),
                ("bahuang", 1),
                ("sanhuan", 2),
                ("sanchai", 3),
                ("suixing", 7),
                ("shengtaiji", 8),
                ("renjian", 12),
            ]
        } else {
            &[
                ("shang", 3),
                ("jue", 4),
                ("shuying", 10),
                ("gaoshan", 8),
                ("yu", 7),
                ("bianzhi", 6),
                ("biangong", 2),
            ]
        };
        let skill = |name: &str| {
            skills
                .iter()
                .find(|s| s.0 == name)
                .map(|s| s.1)
                .ok_or_else(|| format!("unsupported skill {name}"))
        };
        let buffs: &[(&str, u16, bool)] = if tx {
            &[("ziqi", 1, false), ("dieren", 0, true)]
        } else {
            &[
                ("shang", 0, true),
                ("jue", 1, true),
                ("qufeng", 8, false),
                ("yangchun", 10, false),
            ]
        };
        for raw in text
            .lines()
            .map(str::trim)
            .filter(|s| !s.is_empty() && !s.starts_with('#'))
        {
            let (cmd, body) = raw
                .split_once(char::is_whitespace)
                .ok_or("missing action")?;
            let command = match cmd {
                "/cast" => 0,
                "/scast" => 1,
                "/fcast" => 2,
                "/sfcast" => 3,
                _ => return Err(format!("unknown command {cmd}")),
            };
            let mut body = body.trim();
            let start = p.conditions.len();
            if let Some(rest) = body.strip_prefix('[') {
                let (expr, action) = rest.split_once(']').ok_or("missing ]")?;
                body = action.trim();
                for group in expr.split('|') {
                    let begin = p.conditions.len();
                    for term in group.split('&') {
                        let term = term.trim();
                        let op = term.find(['<', '>', '=', '!']).unwrap_or(term.len());
                        let (head, tail) = term.split_at(op);
                        let (kind, name) = head.trim().split_once(':').unwrap_or((head.trim(), ""));
                        let mut cmp = Compare::Gt;
                        let mut rhs = 0.0;
                        if !tail.is_empty() {
                            let (sym, c) = [
                                ("<=", Compare::Le),
                                (">=", Compare::Ge),
                                ("!=", Compare::Ne),
                                ("==", Compare::Eq),
                                ("=", Compare::Eq),
                                ("<", Compare::Lt),
                                (">", Compare::Gt),
                            ]
                            .into_iter()
                            .find(|(sym, _)| tail.starts_with(sym))
                            .ok_or("invalid comparator")?;
                            cmp = c;
                            rhs = tail[sym.len()..]
                                .trim()
                                .parse::<f64>()
                                .map_err(|_| "invalid number")?;
                            if !rhs.is_finite() || rhs < 0.0 || rhs > i32::MAX as f64 {
                                return Err("invalid threshold".into());
                            }
                        }
                        let v = match kind {
                            "qidian" => Value::Qi,
                            "rage" | "energy" => Value::Fixed(0.0),
                            "mana" | "life" | "tlife" => Value::Fixed(1.0),
                            "skill_cd" => {
                                rhs *= 16.0;
                                Value::Cooldown(skill(name)?)
                            }
                            "skill_energy" => Value::Energy(skill(name)?),
                            "last_skill" | "notlast_skill" => {
                                if !tail.is_empty() {
                                    return Err("last_skill takes a name".into());
                                }
                                cmp = if kind == "last_skill" {
                                    Compare::Eq
                                } else {
                                    Compare::Ne
                                };
                                rhs = 1.0;
                                Value::Last(skill(name)?)
                            }
                            "buff" | "nobuff" | "tnobuff" | "tbuff" | "bufftime" | "tbufftime" => {
                                let &(_, s, target) = buffs
                                    .iter()
                                    .find(|b| b.0 == name)
                                    .ok_or_else(|| format!("unsupported buff {name}"))?;
                                if kind.contains("nobuff") {
                                    if !tail.is_empty() {
                                        return Err("nobuff takes a name".into());
                                    }
                                    cmp = Compare::Eq;
                                }
                                if kind.ends_with("time") {
                                    rhs *= 16.0;
                                }
                                if target != kind.starts_with('t') {
                                    Value::Fixed(0.0)
                                } else if kind.ends_with("time") {
                                    Value::Duration(s)
                                } else {
                                    Value::Stacks(s)
                                }
                            }
                            _ => return Err(format!("unsupported condition {kind}")),
                        };
                        let c = Condition {
                            value: v,
                            cmp,
                            rhs,
                            yes: p.conditions.len() + 1,
                            no: 0,
                        };
                        if matches!(v, Value::Duration(_) | Value::Cooldown(_)) {
                            p.time.push(c);
                        }
                        p.conditions.push(c);
                    }
                    let end = p.conditions.len();
                    p.conditions[end - 1].yes = YES;
                    for c in &mut p.conditions[begin..end] {
                        c.no = end;
                    }
                }
                let end = p.conditions.len();
                for c in &mut p.conditions[start..] {
                    if c.no == end {
                        c.no = NO;
                    }
                }
            }
            p.lines.push(Line {
                skill: skill(body)?,
                command,
                start,
                end: p.conditions.len(),
            });
            p.force |= command >= 2;
        }
        Ok(p)
    }
    fn value<R: Rules>(&self, r: &R, c: &Core, v: Value) -> f64 {
        match v {
            Value::Qi => r.qi() as f64,
            Value::Fixed(x) => x,
            Value::Stacks(s) => c.stacks(s) as f64,
            Value::Duration(s) => (c.buffs[s as usize].end - c.now).max(0) as f64,
            Value::Cooldown(s) => (r.ready(c, s) - c.now).max(0) as f64,
            Value::Energy(s) => c.energy[s as usize] as f64,
            Value::Last(s) => u8::from(c.last_skill == s) as f64,
        }
    }
    pub fn scan<R: Rules>(
        &self,
        r: &mut R,
        c: &mut Core,
        consumed: &mut [bool],
    ) -> Option<(u32, u16, u8)> {
        if !self.force && r.busy() {
            return None;
        }
        for (idx, l) in self.lines.iter().enumerate() {
            if consumed[idx] {
                continue;
            }
            if l.command >= 2 {
                r.interrupt(c);
            }
            if r.available(c, l.skill) {
                let mut pc = if l.start == l.end { YES } else { l.start };
                while pc < NO {
                    let expr = self.conditions[pc];
                    let v = self.value(r, c, expr.value);
                    let ok = match expr.cmp {
                        Compare::Lt => v < expr.rhs,
                        Compare::Le => v <= expr.rhs,
                        Compare::Eq => v == expr.rhs,
                        Compare::Ne => v != expr.rhs,
                        Compare::Gt => v > expr.rhs,
                        Compare::Ge => v >= expr.rhs,
                    };
                    pc = if ok { expr.yes } else { expr.no };
                }
                if pc == YES && r.perform(c, l.skill) {
                    if l.command & 1 != 0 {
                        consumed[idx] = true;
                    }
                    return Some((idx as u32, l.skill, l.command));
                }
            }
            if l.command & 1 != 0 {
                return None;
            }
        }
        None
    }
    pub fn next<R: Rules>(&self, r: &R, c: &Core, mut next: i32) -> i32 {
        for expr in &self.time {
            let at = match expr.value {
                Value::Cooldown(s) => r.ready(c, s),
                Value::Duration(s) => c.buffs[s as usize].end,
                _ => unreachable!(),
            };
            if at <= c.now {
                continue;
            }
            let mut consider = |x: i64| {
                if x > i64::from(c.now) && x < i64::from(next) {
                    next = x as i32;
                }
            };
            let at = i64::from(at);
            let rhs = expr.rhs;
            match expr.cmp {
                Compare::Gt | Compare::Le => consider(at - rhs.floor() as i64),
                Compare::Ge | Compare::Lt if rhs > 0.0 => consider(at - rhs.ceil() as i64 + 1),
                Compare::Eq | Compare::Ne if rhs.fract() == 0.0 => {
                    consider(at - rhs as i64);
                    if rhs > 0.0 {
                        consider(at - rhs as i64 + 1);
                    }
                }
                _ => {}
            }
        }
        next
    }
}
