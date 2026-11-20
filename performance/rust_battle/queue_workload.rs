mod queue;
use queue::Queue;
use std::{env, fs, time::Instant};
#[link(name = "kernel32")]
unsafe extern "system" {
    fn GetCurrentThread() -> isize;
    fn SetThreadAffinityMask(thread: isize, mask: usize) -> usize;
}
#[derive(Clone, Copy)]
struct Op {
    tag: u32,
    id: u32,
    value: i32,
}
fn updates(q: &mut Queue, ops: &[Op], i: &mut usize) {
    while let Some(o) = ops.get(*i) {
        match o.tag {
            1 => q.arm(o.id as usize, o.value),
            2 => q.cancel(o.id as usize),
            _ => break,
        }
        *i += 1;
    }
}
fn replay(q: &mut Queue, ops: &[Op], check: &mut u64) {
    let mut i = 0;
    while i < ops.len() {
        updates(q, ops, &mut i);
        if i == ops.len() {
            break;
        }
        let o = ops[i];
        i += 1;
        match o.tag {
            0 => q.reset(),
            3 => {
                let mut dispatched = 0;
                while let Some(id) = q.pop_due() {
                    dispatched += 1;
                    assert!(dispatched <= 100000);
                    assert!(ops[i].tag == 4 && ops[i].id as usize == id);
                    i += 1;
                    *check = check.wrapping_mul(31).wrapping_add(id as u64);
                    updates(q, ops, &mut i);
                }
                assert_eq!(ops[i].tag, 5);
                i += 1;
            }
            6 => assert_eq!(q.next().map(|n| n - q.now).unwrap_or(-1), o.value),
            7 => {
                let frame = q.now.checked_add(o.value).unwrap();
                assert!(o.value >= 0 && q.next().is_none_or(|n| frame <= n));
                q.now = frame;
            }
            _ => panic!("unknown operation"),
        }
    }
}
fn main() {
    assert_ne!(unsafe { SetThreadAffinityMask(GetCurrentThread(), 4) }, 0);
    let args: Vec<_> = env::args().collect();
    let bytes = fs::read(&args[1]).unwrap();
    let n = u32::from_le_bytes(bytes[..4].try_into().unwrap()) as usize;
    let priorities = bytes[4..4 + 2 * n]
        .chunks_exact(2)
        .map(|p| u16::from_le_bytes(p.try_into().unwrap()))
        .collect::<Vec<_>>();
    let data = &bytes[4 + 2 * n..];
    assert_eq!(data.len() % 12, 0);
    let ops = data
        .chunks_exact(12)
        .map(|o| Op {
            tag: u32::from_le_bytes(o[..4].try_into().unwrap()),
            id: u32::from_le_bytes(o[4..8].try_into().unwrap()),
            value: i32::from_le_bytes(o[8..].try_into().unwrap()),
        })
        .collect::<Vec<_>>();
    let mut q = Queue::new(&priorities);
    let mut check = 0;
    replay(&mut q, &ops, &mut check);
    check = 0;
    let reps = args[2].parse::<usize>().unwrap();
    let t = Instant::now();
    for _ in 0..reps {
        replay(&mut q, &ops, &mut check);
    }
    println!("{} {}", t.elapsed().as_secs_f64(), check);
}
