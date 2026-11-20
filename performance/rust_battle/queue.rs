// Indexed min-heap. A timer has at most one pending entry even when rearmed.
const IDLE: usize = usize::MAX;
#[derive(Clone, Copy)]
struct Entry {
    key: u64,
    id: usize,
}
struct Timer {
    order: u64,
    position: usize,
    priority: u16,
}
pub struct Queue {
    timers: Vec<Timer>,
    heap: Vec<Entry>,
    sequence: u64,
    pub now: i32,
}
impl Queue {
    pub fn new(priorities: &[u16]) -> Self {
        let timers = priorities
            .iter()
            .map(|&priority| Timer {
                order: 0,
                position: IDLE,
                priority,
            })
            .collect();
        Self {
            timers,
            heap: Vec::with_capacity(priorities.len()),
            sequence: 0,
            now: 0,
        }
    }
    pub fn reset(&mut self) {
        self.heap.clear();
        self.sequence = 0;
        self.now = 0;
        for timer in &mut self.timers {
            timer.position = IDLE;
        }
    }
    fn before(&self, a: Entry, b: Entry) -> bool {
        a.key < b.key || (a.key == b.key && self.timers[a.id].order < self.timers[b.id].order)
    }
    fn place(&mut self, pos: usize, entry: Entry) {
        self.heap[pos] = entry;
        self.timers[entry.id].position = pos;
    }
    fn up(&mut self, mut pos: usize, entry: Entry) {
        while pos > 0 {
            let parent = (pos - 1) / 2;
            let prev = self.heap[parent];
            if !self.before(entry, prev) {
                break;
            }
            self.place(pos, prev);
            pos = parent;
        }
        self.place(pos, entry);
    }
    fn down(&mut self, mut pos: usize, entry: Entry) {
        while pos < self.heap.len() / 2 {
            let mut child = pos * 2 + 1;
            if child + 1 < self.heap.len() && self.before(self.heap[child + 1], self.heap[child]) {
                child += 1;
            }
            let prev = self.heap[child];
            if !self.before(prev, entry) {
                break;
            }
            self.place(pos, prev);
            pos = child;
        }
        self.place(pos, entry);
    }
    fn remove(&mut self, pos: usize) {
        let removed = self.heap[pos];
        let moved = self.heap.pop().unwrap();
        self.timers[removed.id].position = IDLE;
        if pos < self.heap.len() {
            if pos > 0 && self.before(moved, self.heap[(pos - 1) / 2]) {
                self.up(pos, moved);
            } else {
                self.down(pos, moved);
            }
        }
    }
    pub fn arm(&mut self, id: usize, deadline: i32) {
        assert!(deadline >= self.now);
        let timer = &mut self.timers[id];
        let order = self.sequence;
        self.sequence += 1;
        timer.order = order;
        let key = (deadline as u64) << 32 | (timer.priority as u64) << 16 | order.min(65535);
        let pos = timer.position;
        let entry = Entry { id, key };
        if pos == IDLE {
            let end = self.heap.len();
            self.heap.push(entry);
            self.up(end, entry);
        } else if key < self.heap[pos].key {
            self.up(pos, entry);
        } else {
            self.down(pos, entry);
        }
    }
    pub fn cancel(&mut self, id: usize) {
        let pos = self.timers[id].position;
        if pos != IDLE {
            self.remove(pos);
        }
    }
    pub fn next(&self) -> Option<i32> {
        self.heap.first().map(|entry| (entry.key >> 32) as i32)
    }
    pub fn pop_due(&mut self) -> Option<usize> {
        if self.next()? > self.now {
            return None;
        }
        let id = self.heap[0].id;
        self.remove(0);
        Some(id)
    }
}
