/**
 * Project: 
 * File: Expression.h
 * Description:
 * Created Date: 2023-07-22 13:41:19
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-28 12:07:06
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_REGEX_H__
#define __JX3DPS_REGEX_H__

#include "Global/Defs.h"
#include "Global/Id.h"
#include "Global/Types.h"

#include "Expression.h"

namespace JX3DPS {

namespace Regex {

Error_t ParseToExprSkillsHash(const std::list<std::pair<std::string, std::list<std::string>>> &strs, ExprSkillsHash &exprSkillsHash);

Error_t ParseToExprSkill(const std::string &str, ExprSkill &exprSkill);

Error_t ParseToExprEvents(const std::list<std::string> &strs, ExprEvents &exprEvents);

Error_t ParseToBuff3rds(const std::list<std::string> &strs, std::list<Id_t> &buff3rds);

Error_t ParseToTotalFrames(const std::list<std::string> &strs, Frame_t &totalFrames);

Error_t ParseToExprIf(const std::string &str, ExprIf &exprIf);

Error_t ParseToFrame(const std::string &str, Frame_t &frame);

Error_t ParseToAddTarget(const std::string &str, ExprEvents &exprEvents);

Error_t ParseToChangeTarget(const std::string &str, ExprEvent &exprEvent);

Error_t ParseToSetTarget(const std::string &str, ExprEvent &exprEvent);

Error_t ParseToAddBuff(const std::string &str, ExprEvent &exprEvent);

Error_t ParseToClearBuff(const std::string &str, ExprEvent &exprEvent);

Error_t ParseToStop(const std::string &str, ExprEvent &exprEvent);

Error_t ParseToContinue(const std::string &str, ExprEvent &exprEvent);

Error_t ParseToEnd(const std::string &str, ExprEvent &exprEvent);

} // namespace Regex

} // namespace JX3DPS

#endif // __JX3DPS_REGEX_H__