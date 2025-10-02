// JX3 宏编辑器 - 支持断点和语法高亮
class JX3MacroEditor {
    constructor() {
        this.editor = document.getElementById('editor');
        this.highlightLayer = document.getElementById('highlight-layer');
        this.gutter = document.getElementById('gutter');
        this.highlighter = new JX3MacroHighlighter();
        this.breakpoints = new Set();

        // 示例代码
        this.editor.value = `## 主循环宏 - 基础输出
/scast [buff:无我无剑] 八荒归元
/scast [nobuff:无我无剑&skill_cd:八荒归元<8] 无我无剑
/scast [bufftime:无我无剑<2&tbuff:破>2] 三环套月
/scast [rage>=20] 人剑合一
/cast [tbuff:流血] 碎星辰
/cast [nobuff:太极] 太极无极
/cast 三柱剑法

## 爆发宏 - 高伤输出
/fcast [buff:玄门]&qidian>7] 两仪化形
/scast [buff:持盾] 八荒归元
/scast [buff:梦悠=4] 疾如风
/cast [skill_energy:盾飞>=2] 盾飞
/cast [nearby_enemy>2] 风来吴山

## 条件判定示例
/cast [life<0.3] 嗑如虎
/cast [tlife<0.1] 闻须弥
/cast [mana<0.4] 碧水澄天
/cast [tbufftime:流血<2] 龙吟
/cast [skill_cd:八荒归元<4.5] 无我无剑

## 事件语句示例（时间格式）
00:05.0 /add_target id=1 level=124 shield=27550 distance=3 lifetime=50
00:10.5 /set_target id=1 distance=4
00:30.0 /add_buff id=0 name=玄门 stack_num=3 duration=20-23
01:00.0 /change_target id=1
02:30.0 /set_target id=1 dead
03:00.0 /end`;

        this.init();
    }

    init() {
        // 初始化编辑器
        this.updateHighlight();
        this.updateGutter();

        // 绑定事件
        this.editor.addEventListener('input', () => {
            this.updateHighlight();
            this.updateGutter();
        });

        this.editor.addEventListener('scroll', () => {
            this.highlightLayer.scrollTop = this.editor.scrollTop;
            this.highlightLayer.scrollLeft = this.editor.scrollLeft;
            this.gutter.scrollTop = this.editor.scrollTop;
        });

        this.gutter.addEventListener('click', (e) => {
            this.handleGutterClick(e);
        });

        // 同步滚动
        this.editor.addEventListener('scroll', () => {
            this.syncScroll();
        });
    }

    updateHighlight() {
        const code = this.editor.value;
        this.highlightLayer.innerHTML = this.highlighter.highlight(code);
    }

    updateGutter() {
        const lines = this.editor.value.split('\n');
        let gutterHtml = '';

        for (let i = 0; i < lines.length; i++) {
            const lineNumber = i + 1;
            const hasBreakpoint = this.breakpoints.has(lineNumber);
            const breakpointClass = hasBreakpoint ? 'breakpoint' : '';
            gutterHtml += `<div class="gutter-line ${breakpointClass}" data-line="${lineNumber}">
                ${hasBreakpoint ? '<span class="breakpoint-dot"></span>' : ''}
                <span class="line-number">${lineNumber}</span>
            </div>`;
        }

        this.gutter.innerHTML = gutterHtml;
    }

    handleGutterClick(e) {
        const gutterLine = e.target.closest('.gutter-line');
        if (!gutterLine) return;

        const lineNumber = parseInt(gutterLine.dataset.line);
        this.toggleBreakpoint(lineNumber);
    }

    toggleBreakpoint(lineNumber) {
        if (this.breakpoints.has(lineNumber)) {
            this.breakpoints.delete(lineNumber);
            console.log(`Breakpoint removed: line ${lineNumber}`);
        } else {
            this.breakpoints.add(lineNumber);
            console.log(`Breakpoint added: line ${lineNumber}`);
        }
        this.updateGutter();
    }

    syncScroll() {
        this.highlightLayer.scrollTop = this.editor.scrollTop;
        this.highlightLayer.scrollLeft = this.editor.scrollLeft;
        this.gutter.scrollTop = this.editor.scrollTop;
    }

    getBreakpoints() {
        return Array.from(this.breakpoints).sort((a, b) => a - b);
    }
}

// 初始化编辑器
document.addEventListener('DOMContentLoaded', () => {
    window.editor = new JX3MacroEditor();
});
