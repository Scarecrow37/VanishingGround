#include "ReflectHelper.h"
#include "pch.h"

//설정값 관리용 전역 변수 재컴파일 방지를 위해 cpp에 초기값 선언.
using namespace ReflectHelper::ImGuiDraw::InputAutoSetting;

//Int 설정
int InputAutoSettingInt::step = 0;
int InputAutoSettingInt::step_fast = 0;
ImGuiInputTextFlags InputAutoSettingInt::flags = 0;
InputAutoSettingInt Int;

//Float 설정
float InputAutoSettingFloat::step = 0;
float InputAutoSettingFloat::step_fast = 0;
std::string InputAutoSettingFloat::format = "%.3f";
ImGuiInputTextFlags InputAutoSettingFloat::flags = 0;
InputAutoSettingFloat Float;

//Double 설정
double InputAutoSettingDouble::step = 0;
double InputAutoSettingDouble::step_fast = 0;
std::string InputAutoSettingDouble::format = "%.6f";
ImGuiInputTextFlags InputAutoSettingDouble::flags = 0;
InputAutoSettingDouble Double;

//String 설정
ImGuiInputTextFlags InputAutoSettingString::flags = 0;
ImGuiInputTextCallback InputAutoSettingString::callback = nullptr;
void* InputAutoSettingString::user_data = nullptr;
InputAutoSettingString String;

//Vector3 설정
ImGuiInputTextFlags InputAutoSettingVector3::falgs = 0;