// JX3 宏语法高亮器 - 与 C++ 版本保持一致
class JX3MacroHighlighter {
    constructor() {
        // VSCode Dark 主题颜色
        this.colors = {
            comment: '#6a9955',      // 注释 - 绿色
            command: '#c586c0',      // 命令 - 紫色
            keyword: '#569cd6',      // 关键字 - 蓝紫色
            number: '#b5cea8',       // 数字 - 浅绿色
            operator: '#d4d4d4',     // 操作符 - 灰白色
            skillName: '#dcdcaa',    // 技能名/中文 - VSCode函数标准黄
        };

        // 命令列表
        this.commands = [
            'cast', 'fcast', 'scast', 'sfcast',
            'add_target', 'set_target', 'change_target',
            'add_buff', 'clear_buff',
            'stop', 'continue', 'end'
        ];

        // 带冒号的关键字
        this.colonKeywords = [
            'buff', 'nobuff', 'bufftime',
            'qidian', 'energy', 'sun', 'moon', 'sun_power', 'moon_power',
            'skill_energy', 'skill', 'noskill', 'last_skill', 'skill_cd',
            'npclevel', 'nearby_enemy', 'skill_notin_cd', 'yaoxing',
            'tbuff', 'tnobuff', 'tbufftime',
            'Duff', 'Buff', 'ebufftime'
        ];

        // 带比较运算符的关键字
        this.comparisonKeywords = [
            'qidian', 'life', 'mana', 'rage', 'tlife', 'tmana', 'trage', 'nearby_enemy'
        ];

        // 带等号的关键字
        this.equalKeywords = [
            'id', 'name', 'stack_num', 'duration', 'distance', 'shield', 'level'
        ];

        // 独立关键字
        this.standaloneKeywords = ['dead'];
    }

    highlight(text) {
        let html = '';
        const lines = text.split('\n');

        for (let line of lines) {
            // 检查是否是注释（最后处理，覆盖其他规则）
            const commentMatch = line.match(/^(.*)((\/\/|##?).*)/);

            if (commentMatch) {
                // 行中有注释
                const beforeComment = commentMatch[1];
                const comment = commentMatch[2];
                html += this.highlightLine(beforeComment);
                html += `<span style="color: ${this.colors.comment}">${this.escapeHtml(comment)}</span>\n`;
            } else {
                // 普通行
                html += this.highlightLine(line) + '\n';
            }
        }

        return html;
    }

    highlightLine(line) {
        // 按优先级顺序应用高亮规则
        let parts = [{ text: line, type: 'normal' }];

        // 1. 命令
        parts = this.applyPattern(parts, new RegExp(`/(${this.commands.join('|')})\\b`, 'g'), 'command');

        // 2. 时间格式
        parts = this.applyPattern(parts, /\b\d+:\d+\.\d+\b/g, 'number');

        // 3. 数字
        parts = this.applyPattern(parts, /\b\d+\.?\d*\b/g, 'number');

        // 4. 带冒号的关键字
        for (let keyword of this.colonKeywords) {
            parts = this.applyPattern(parts, new RegExp(`\\b${keyword}(?=\\s*:)`, 'g'), 'keyword');
        }

        // 5. 带比较运算符的关键字
        for (let keyword of this.comparisonKeywords) {
            parts = this.applyPattern(parts, new RegExp(`\\b${keyword}(?=\\s*[><=])`, 'g'), 'keyword');
        }

        // 6. 带等号的关键字
        for (let keyword of this.equalKeywords) {
            parts = this.applyPattern(parts, new RegExp(`\\b${keyword}(?=\\s*=)`, 'g'), 'keyword');
        }

        // 7. 独立关键字
        for (let keyword of this.standaloneKeywords) {
            parts = this.applyPattern(parts, new RegExp(`\\b${keyword}\\b`, 'g'), 'keyword');
        }

        // 8. 操作符
        parts = this.applyPattern(parts, /[\[\]&|=><~:]/g, 'operator');

        // 9. 中文字符（技能名/buff名）
        parts = this.applyPattern(parts, /[\u4e00-\u9fa5]+/g, 'skillName');

        // 构建最终 HTML
        return parts.map(part => {
            if (part.type === 'normal') {
                return this.escapeHtml(part.text);
            } else {
                return `<span style="color: ${this.colors[part.type]}">${this.escapeHtml(part.text)}</span>`;
            }
        }).join('');
    }

    applyPattern(parts, pattern, type) {
        const newParts = [];
        for (let part of parts) {
            if (part.type !== 'normal') {
                newParts.push(part);
                continue;
            }

            let lastIndex = 0;
            let match;
            const text = part.text;
            pattern.lastIndex = 0;

            while ((match = pattern.exec(text)) !== null) {
                // 添加匹配前的文本
                if (match.index > lastIndex) {
                    newParts.push({
                        text: text.substring(lastIndex, match.index),
                        type: 'normal'
                    });
                }
                // 添加匹配的文本
                newParts.push({
                    text: match[0],
                    type: type
                });
                lastIndex = match.index + match[0].length;
            }

            // 添加剩余文本
            if (lastIndex < text.length) {
                newParts.push({
                    text: text.substring(lastIndex),
                    type: 'normal'
                });
            }
        }
        return newParts.length > 0 ? newParts : parts;
    }

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}
