#include "ReflectHelper.h"
#include "pch.h"

using namespace ReflectHelper::ImGuiDraw;

//설정값 관리용 전역 변수 재컴파일 방지를 위해 cpp에 초기값 선언. 
//Int 설정
int InputAutoSetting::Int::step = 0;
int InputAutoSetting::Int::step_fast = 0;
ImGuiInputTextFlags InputAutoSetting::Int::flags = 0;

//Float 설정
float InputAutoSetting::Float::step = 0;
float InputAutoSetting::Float::step_fast = 0;
std::string InputAutoSetting::Float::format = "%.3f";
ImGuiInputTextFlags InputAutoSetting::Float::flags = 0;

//Double 설정
double InputAutoSetting::Double::step = 0;
double InputAutoSetting::Double::step_fast = 0;
std::string InputAutoSetting::Double::format = "%.6f";
ImGuiInputTextFlags InputAutoSetting::Double::flags = 0;

//String 설정
ImGuiInputTextFlags InputAutoSetting::String::flags = 0;
ImGuiInputTextCallback InputAutoSetting::String::callback = nullptr;
void* InputAutoSetting::String::user_data = nullptr;

//Vector2 설정
float InputAutoSetting::Vector2::v_speed            = 0.1f;
float InputAutoSetting::Vector2::v_min              = 0.f;
float InputAutoSetting::Vector2::v_max              = 0.f;
std::string InputAutoSetting::Vector2::format       = "%.3f";
ImGuiSliderFlags InputAutoSetting::Vector2::flags   = 0;

//Vector3 설정
float InputAutoSetting::Vector3::v_speed = 0.1f;
float InputAutoSetting::Vector3::v_min = 0.f;
float InputAutoSetting::Vector3::v_max = 0.f;
std::string InputAutoSetting::Vector3::format = "%.3f";
ImGuiSliderFlags InputAutoSetting::Vector3::flags = 0;

//Vector4 설정
float InputAutoSetting::Vector4::v_speed = 0.1f;
float InputAutoSetting::Vector4::v_min = 0.f;
float InputAutoSetting::Vector4::v_max = 0.f;
std::string InputAutoSetting::Vector4::format = "%.3f";
ImGuiSliderFlags InputAutoSetting::Vector4::flags = 0;

void ReflectHelper::ImGuiDraw::Private::EngineLog(int logLevel, std::string_view message, std::source_location location)
{
    Global::engineCore->Logger.Log(logLevel, message, location);
}
