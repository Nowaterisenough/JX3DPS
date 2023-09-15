/**
 * Project: JX3DPS
 * File: JX3DPSJsonParser.cpp
 * Description:
 * Created Date: 2023-08-10 00:05:57
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-13 05:25:06
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "JX3DPSJsonParser.h"

#include <fstream>

QString JX3DPS::Simulator::JsonParser::JX3IconPath(int id)
{
    return QString(":/resources/pics/JX3/Icons/%1.png").arg(id);
}

void JX3DPS::Simulator::JsonParser::LoadConfig(const std::string &config, nlohmann::ordered_json &json)
{
    std::ifstream ifs(config);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open config file.");
    }
    ifs >> json;
    ifs.close();
}

void JX3DPS::Simulator::JsonParser::ParseJsonToTalentItemInfos(
    const nlohmann::ordered_json               &json,
    JX3DPS::ClassType                           classType,
    std::vector<std::list<ComboBox::ItemInfo>> &talents,
    std::list<std::string>                     &defaults)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &talent : item["Talents"].items()) {
                std::list<ComboBox::ItemInfo> talentItemInfos;
                for (auto &itemInfo : talent.value()) {
                    ComboBox::ItemInfo itemInfo_;
                    itemInfo_.name     = itemInfo["Name"].get<std::string>().c_str();
                    itemInfo_.iconPath = JX3IconPath(itemInfo["Icon"].get<int>());
                    itemInfo_.description = itemInfo["Description"].get<std::string>().c_str();
                    talentItemInfos.push_back(itemInfo_);
                }
                talents.push_back(talentItemInfos);
            }

            for (auto &item : item["Default"].items()) {
                if (item.key() == "Talents") {
                    for (auto &iter : item.value()) {
                        defaults.push_back(iter.get<std::string>());
                    }
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToRecipeItemInfos(
    const nlohmann::ordered_json                                     &json,
    JX3DPS::ClassType                                                 classType,
    std::list<std::pair<std::string, std::list<CheckBox::ItemInfo>>> &recipes,
    std::list<std::string>                                           &defaults)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &recipe : item["Recipes"].items()) {
                std::list<CheckBox::ItemInfo> recipeItemInfos;
                for (auto &itemInfo : recipe.value()) {
                    CheckBox::ItemInfo itemInfo_;
                    itemInfo_.name     = itemInfo["Name"].get<std::string>().c_str();
                    itemInfo_.iconPath = JX3IconPath(itemInfo["Icon"].get<int>());
                    itemInfo_.description = itemInfo["Description"].get<std::string>().c_str();
                    recipeItemInfos.push_back(itemInfo_);
                }
                recipes.emplace_back(recipe.key(), recipeItemInfos);
            }

            for (auto &item : item["Default"].items()) {
                if (item.key() == "Recipes") {
                    for (auto &it : item.value().items()) {
                        for (auto &iter : it.value()) {
                            defaults.push_back(iter.get<std::string>());
                        }
                    }
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToSkills(
    const nlohmann::ordered_json                              &json,
    JX3DPS::ClassType                                          classType,
    std::list<std::pair<std::string, std::list<std::string>>> &skills)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &item : item["Default"].items()) {
                if (item.key() == "SkillsExpression") {
                    for (auto &[skillName, exprs] : item.value().items()) {
                        std::list<std::string> exprs_ = exprs.get<std::list<std::string>>();
                        skills.emplace_back(skillName, exprs_);
                    }
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToEvents(const nlohmann::ordered_json &json,
                                                      JX3DPS::ClassType       classType,
                                                      std::list<std::string> &events)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &item : item["Default"].items()) {
                if (item.key() == "EventsExpression") {
                    events = item.value().get<std::list<std::string>>();
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToPermanents(
    const nlohmann::ordered_json                                   &json,
    JX3DPS::ClassType                                               classType,
    std::unordered_map<std::string, CheckBox::ItemInfo>            &permanents1,
    std::unordered_map<std::string, std::list<ComboBox::ItemInfo>> &permanents2)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &[key, value] : item["Permanents"].items()) {
                if (key == "Others") {
                    for (auto &permanent : value) {
                        CheckBox::ItemInfo itemInfo;
                        itemInfo.name     = permanent["Name"].get<std::string>().c_str();
                        itemInfo.iconPath = JX3IconPath(permanent["Icon"].get<int>());
                        itemInfo.description = permanent["Description"].get<std::string>().c_str();
                        permanents1.emplace(itemInfo.name.toStdString(), itemInfo);
                    }
                } else {
                    std::list<ComboBox::ItemInfo> itemInfos;
                    for (auto &permanent : value) {
                        ComboBox::ItemInfo itemInfo;
                        itemInfo.name     = permanent["Name"].get<std::string>().c_str();
                        itemInfo.iconPath = JX3IconPath(permanent["Icon"].get<int>());
                        itemInfo.description = permanent["Description"].get<std::string>().c_str();
                        itemInfos.emplace_back(itemInfo);
                    }
                    permanents2.emplace(key, itemInfos);
                }
            }
        }
    }
}

void JX3DPS::Simulator::JsonParser::LoadJsonPermanent(const nlohmann::ordered_json &json,
                                                      JX3DPS::ClassType       classType,
                                                      const std::string      &name,
                                                      nlohmann::ordered_json &out)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &[key, value] : item["Permanents"].items()) {
                for (auto &permanent : value) {
                    std::string temp = permanent["Name"].get<std::string>();
                    if (name == temp) {
                        out = permanent;
                        return;
                    }
                }
            }
        }
    }
}

// 计算两个颜色向量之间的欧几里得距离
float Distance(const std::vector<float> &a, const std::vector<float> &b)
{
    float sum = 0;
    for (int i = 0; i < a.size(); ++i) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return std::sqrt(sum);
}

// 将颜色向量分配到最近的聚类中心
void AssignClusters(const std::vector<std::vector<float>> &colors,
                    const std::vector<std::vector<float>> &centers,
                    std::vector<int>                      &labels)
{
    for (int i = 0; i < colors.size(); ++i) {
        float minDistance = std::numeric_limits<float>::max();
        int   minIndex    = -1;
        for (int j = 0; j < centers.size(); ++j) {
            float d = Distance(colors[i], centers[j]);
            if (d < minDistance) {
                minDistance = d;
                minIndex    = j;
            }
        }
        labels[i] = minIndex;
    }
}

// 计算聚类中心
void ComputeCenters(const std::vector<std::vector<float>> &colors,
                    const std::vector<int>                &labels,
                    std::vector<std::vector<float>>       &centers)
{
    std::vector<int> counts(centers.size(), 0);
    for (int i = 0; i < colors.size(); ++i) {
        int label = labels[i];
        for (int j = 0; j < colors[i].size(); ++j) {
            centers[label][j] += colors[i][j];
        }
        ++counts[label];
    }
    for (int i = 0; i < centers.size(); ++i) {
        if (counts[i] > 0) {
            for (int j = 0; j < centers[i].size(); ++j) {
                centers[i][j] /= counts[i];
            }
        }
    }
}

// K-Means 聚类算法
void Kmeans(const std::vector<std::vector<float>> &colors,
            int                                    clusterCount,
            std::vector<int>                      &labels,
            std::vector<std::vector<float>>       &centers)
{
    // 随机初始化聚类中心
    // std::random_device                 rd;
    // std::mt19937                       gen(rd());
    // std::uniform_int_distribution<int> dist(0, colors.size() - 1);
    centers.resize(clusterCount);
    for (int i = 0; i < clusterCount; ++i) {
        centers[i] = colors[0];
    }

    // 迭代聚类过程
    labels.resize(colors.size());
    for (int iter = 0; iter < 10; ++iter) {
        AssignClusters(colors, centers, labels);
        ComputeCenters(colors, labels, centers);
    }
}

QColor GetDominantColor(const QPixmap &pixmap)
{
    // 将 QPixmap 转换为 QImage
    QImage image = pixmap.toImage();

    // 将图像转换为 32 位 ARGB 格式
    QImage argbImage = image.convertToFormat(QImage::Format_ARGB32);

    // 获取图像像素数据
    const unsigned char *pixels     = argbImage.constBits();
    int                  pixelCount = argbImage.width() * argbImage.height();

    // 将像素数据转换为颜色向量
    std::vector<std::vector<float>> colors(pixelCount, std::vector<float>(3));
    for (int i = 0; i < pixelCount; ++i) {
        colors[i][0] = pixels[4 * i + 2];
        colors[i][1] = pixels[4 * i + 1];
        colors[i][2] = pixels[4 * i];
    }

    // 使用 K-Means 算法聚类颜色向量
    int                             clusterCount = 5;
    std::vector<int>                labels;
    std::vector<std::vector<float>> centers;
    Kmeans(colors, clusterCount, labels, centers);

    // 统计每个颜色簇的像素数量
    std::unordered_map<int, int> colorCounts;
    for (int i = 0; i < pixelCount; ++i) {
        ++colorCounts[labels[i]];
    }

    // 找到像素数量最多的颜色簇
    int dominantCluster = 0;
    int maxCount        = 0;
    for (const auto &pair : colorCounts) {
        if (pair.second > maxCount) {
            dominantCluster = pair.first;
            maxCount        = pair.second;
        }
    }

    // 返回主色调
    std::vector<float> dominantColor = centers[dominantCluster];
    return QColor::fromRgb(dominantColor[0], dominantColor[1], dominantColor[2]);
}

bool JX3DPS::Simulator::JsonParser::LoadJsonTimeLine(const nlohmann::ordered_json &json,
                                                     const nlohmann::ordered_json &config,
                                                     std::list<SkillItem::Info> &skillInfos)
{
    if (json.find("TimeLine") == json.end()) {
        return false;
    }

    std::string                          className = json["ClassType"].get<std::string>();
    std::unordered_map<std::string, int> skillMap;
    for (auto &item : config["ClassType"]) {
        if (item["Name"].get<std::string>() == className) {
            for (auto &item : item["Skills"].items()) {
                skillMap.emplace(item.value()["Name"].get<std::string>(),
                                 item.value()["Icon"].get<int>());
            }
        }
    }

    for (auto &item : json["TimeLine"]) {
        SkillItem::Info skillInfo;
        skillInfo.timeStamp = item["TimeStamp"].get<int>();
        if (item.find("Skills") == item.end()) {
            continue;
        }
        for (auto &skill : item["Skills"].items()) {
            SkillItem::SkillInfo skillInfo_;
            skillInfo_.damage     = skill.value()["Damage"].get<int>();
            skillInfo_.rollResult = skill.value()["RollResult"].get<int>();
            skillInfo_.name       = skill.value()["Name"].get<std::string>().c_str();
            skillInfo_.type       = skill.value()["Type"].get<int>();
            skillInfo_.icon       = skillMap[skillInfo_.name.toStdString()];
            skillInfo_.pixmap     = QPixmap(JX3IconPath(skillInfo_.icon));
            skillInfo_.color      = GetDominantColor(skillInfo_.pixmap);
            skillInfo_.roll       = skillInfo_.rollResult == 0 ? "命中" : "会心";

            skillInfo.skills.push_back(skillInfo_);
        }
        skillInfos.push_back(skillInfo);
    }

    return true;
}

void JX3DPS::Simulator::JsonParser::ParseJsonToClassTypeItemInfos(const nlohmann::ordered_json &json,
                                                                  std::list<ComboBox::ItemInfo> &itemInfos)
{
    for (auto &item : json["ClassType"]) {
        ComboBox::ItemInfo itemInfo;
        itemInfo.name        = item["Name"].get<std::string>().c_str();
        itemInfo.iconPath    = JX3IconPath(item["Icon"].get<int>());
        itemInfo.description = item["Description"].get<std::string>().c_str();
        itemInfos.push_back(itemInfo);
    }
}

void JX3DPS::Simulator::JsonParser::ParseJsonToDefaultAttribute(
    const nlohmann::ordered_json         &json,
    JX3DPS::ClassType                     classType,
    std::unordered_map<std::string, int> &attributes)
{
    for (auto &item : json["ClassType"]) {
        if (item["Name"].get<std::string>() == JX3DPS::CLASS_NAME[static_cast<int>(classType)])
        {
            for (auto &[key, value] : item["Default"].items()) {
                if (key == "Attribute") {
                    for (auto &[key, value] : value.items()) {
                        attributes.emplace(key, value.get<int>());
                    }
                }
            }
        }
    }
}

long long JX3DPS::Simulator::JsonParser::GetTotalDamage(const nlohmann::ordered_json &json)
{
    long long sum = 0;
    for (auto &[targetName, targetDamage] : json.items()) {
        for (auto &[effectName, effectDamage] : targetDamage.items()) {
            for (auto &[subName, subDamage] : effectDamage.items()) {
                for (auto &[levelName, levelDamage] : subDamage.items()) {
                    for (auto &[rollResult, info] : levelDamage.items()) {
                        sum += info["伤害"].get<long long>();
                        sum += info["破招伤害"].get<long long>();
                    }
                }
            }
        }
    }
    return sum;
}

std::list<std::tuple<std::string, long long>> JX3DPS::Simulator::JsonParser::GetTargetDamages(
    const nlohmann::ordered_json &json)
{
    std::list<std::tuple<std::string, long long>> targetDamages;
    long long                                     total = 0;
    targetDamages.emplace_back("总计", total);
    for (auto &[targetName, targetDamage] : json.items()) {
        long long sum = 0;
        for (auto &[effectName, effectDamage] : targetDamage.items()) {
            for (auto &[subName, subDamage] : effectDamage.items()) {
                for (auto &[levelName, levelDamage] : subDamage.items()) {
                    for (auto &[rollResult, info] : levelDamage.items()) {
                        sum += info["伤害"].get<long long>();
                        sum += info["破招伤害"].get<long long>();
                    }
                }
            }
        }
        total += sum;
        targetDamages.emplace_back(targetName, sum);
    }
    std::get<1>(targetDamages.front()) += total;
    return targetDamages;
}

std::unordered_map<std::string, std::tuple<int, long long>> JX3DPS::Simulator::JsonParser::GetTargetEffectDamages(
    const nlohmann::ordered_json &json,
    const std::string            &target)
{
    std::unordered_map<std::string, std::tuple<int, long long>> targetEffectDamages;
    for (auto &[targetName, targetDamage] : json.items()) {
        if (target != targetName && target != "总计") {
            continue;
        }
        for (auto &[effectName, effectDamage] : targetDamage.items()) {
            int       count = 0;
            long long sum   = 0;
            for (auto &[subName, subDamage] : effectDamage.items()) {
                for (auto &[levelName, levelDamage] : subDamage.items()) {
                    for (auto &[rollResult, info] : levelDamage.items()) {
                        sum   += info["伤害"].get<long long>();
                        sum   += info["破招伤害"].get<long long>();
                        count += info["数目"].get<int>();
                    }
                }
            }
            if (targetEffectDamages.find(effectName) != targetEffectDamages.end()) {
                std::get<0>(targetEffectDamages[effectName]) += count;
                std::get<1>(targetEffectDamages[effectName]) += sum;
            } else {
                targetEffectDamages.emplace(effectName, std::tuple<int, long long>(count, sum));
            }
        }
    }
    return targetEffectDamages;
}

std::unordered_map<std::string, std::tuple<int, long long>> JX3DPS::Simulator::JsonParser::GetTargetEffectDamages(
    const nlohmann::ordered_json &json,
    const std::string            &target,
    const std::string            &effect)
{
    std::unordered_map<std::string, std::tuple<int, long long>> targetEffectRollDamages;
    for (auto &[targetName, targetDamage] : json.items()) {
        if (target != targetName && target != "总计") {
            continue;
        }
        for (auto &[effectName, effectDamage] : targetDamage.items()) {
            if (effect != effectName) {
                continue;
            }
            for (auto &[subName, subDamage] : effectDamage.items()) {
                for (auto &[levelName, levelDamage] : subDamage.items()) {
                    for (auto &[rollResult, info] : levelDamage.items()) {
                        int       count  = 0;
                        long long sum    = 0;
                        sum             += info["伤害"].get<long long>();
                        sum             += info["破招伤害"].get<long long>();
                        count           += info["数目"].get<int>();

                        if (targetEffectRollDamages.find(rollResult) !=
                            targetEffectRollDamages.end())
                        {
                            std::get<0>(targetEffectRollDamages[rollResult]) += count;
                            std::get<1>(targetEffectRollDamages[rollResult]) += sum;
                        } else {
                            targetEffectRollDamages.emplace(rollResult,
                                                            std::tuple<int, long long>(count, sum));
                        }
                    }
                }
            }
        }
    }
    return targetEffectRollDamages;
}
