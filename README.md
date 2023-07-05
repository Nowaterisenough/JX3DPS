# JX3DPS

| [Windows][win-link]| [Ubuntu][ubuntu-link]|[MacOS][macos-link]|
|---------------|---------------|-----------------|
| ![win-badge]  | ![ubuntu-badge]      | ![macos-badge] |


|[License][license-link]| [Release][release-link]|[Download][download-link]|[Issues][issues-link]|
|-----------------|-----------------|-----------------|-----------------|
|![license-badge] |![release-badge] | ![download-badge]|![issues-badge]|

[win-link]: https://github.com/Nowaterisenough/JX3DPS/actions?query=workflow%3AWindows "WindowsAction"
[win-badge]: https://github.com/Nowaterisenough/JX3DPS/workflows/Windows/badge.svg  "Windows"

[ubuntu-link]: https://github.com/Nowaterisenough/JX3DPS/actions?query=workflow%3AUbuntu "UbuntuAction"
[ubuntu-badge]: https://github.com/Nowaterisenough/JX3DPS/workflows/Ubuntu/badge.svg "Ubuntu"

[macos-link]: https://github.com/Nowaterisenough/JX3DPS/actions?query=workflow%3AMacOS "MacOSAction"
[macos-badge]: https://github.com/Nowaterisenough/JX3DPS/workflows/MacOS/badge.svg "MacOS"

[release-link]: https://github.com/Nowaterisenough/JX3DPS/releases "Release status"
[release-badge]: https://img.shields.io/github/release/Nowaterisenough/JX3DPS.svg?style=flat-square "Release status"

[download-link]: https://github.com/Nowaterisenough/JX3DPS/releases/latest "Download status"
[download-badge]: https://img.shields.io/github/downloads/Nowaterisenough/JX3DPS/total.svg?style=flat-square "Download status"

[license-link]: https://github.com/Nowaterisenough/JX3DPS/blob/master/LICENSE "LICENSE"
[license-badge]: https://img.shields.io/badge/license-MIT-blue.svg "MIT"

[issues-link]: https://github.com/Nowaterisenough/JX3DPS/issues "Issues"
[issues-badge]: https://img.shields.io/badge/github-issues-red.svg?maxAge=60 "Issues"


## For Developer
1. git代码仓
```git
git clone --recurse-submodules git@github.com:Nowaterisenough/JX3DPS.git
```
2. CMake编译

TODO
1. 转火方面，首先需要一个战斗时间随机数生成，来模拟实战中不知道能打多久的转火目标。生成2个随机数，第1个随机数说明总击杀时间，第2个随机数作为击杀前的一个标志来提醒快要被击杀（比如只剩10%血了，长时间的持续技能就要通过这个标准来考虑是否改变策略）。
举例：总击杀时间rand12~18秒，最后rand第2~5秒提示快要被击杀。

2. 副本干扰技能方面，需要有若干标签（可以内置，并且允许让用户自定义）。举例，一个每2秒判定一次，判定期间玩家不允许为禁止状态的副本技能，把读条时间长于2秒的技能和短于2秒的技能打上不同标签。
这部分由于实际副本更复杂（部分技能即使读不完也可能有价值，boss技能特殊），所以需要用户自定义。

3. 副本总览方面，需要一个完整的boss技能轴自定义（xx秒发生xx副本事件），并且宏语句允许对副本事件的时间进行相关判定（模拟玩家在已知技能轴时的技能规划）。

4. 宏语句方面，宏需要更多划分，否则逻辑套娃太多。例如宏区分为站桩主循环、转火、应对干扰技能（每个标签独立）这几个大划分，每个大划分之下还允许多个宏并行（类似于多键宏）
