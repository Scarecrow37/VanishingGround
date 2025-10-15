#pragma once
#include "Monster/Common/MonsterCommon.h"

 #define REGISTER_MONSTER_ACTION(CLASS)                                                                             \
 namespace ##CLASS##_##Factory                                                                                      \
 {                                                                                                                  \
     static bool IsRegister = ActionFactory::RegisterActionBuilder<CLASS>();                                        \
 };

namespace Monster
{
    class Action;

    class ActionFactory
    {
        using ActionBuildFunc = std::function<Action*()>;
    public:
        ActionFactory();
        ~ActionFactory();

    public:
        /// <summary>
        /// 주어진 ActionID로부터 새로운 Action 객체를 할당합니다. 삭제는 호출자 책임입니다.
        /// </summary>
        /// <param name="id">생성할 Action 객체의 식별자입니다.</param>
        /// <param name="outAction">생성된 Action 객체의 포인터를 저장할 포인터의 주소입니다.</param>
        /// <returns>Action 객체 생성이 성공하면 true, 실패하면 false를 반환합니다.</returns>
        static bool NewActionFromID(ActionID id, Action** outAction);

        /// <summary>
        /// 액션 빌더를 지정된 액션 ID에 등록합니다.
        /// </summary>
        /// <param name="id">등록할 액션의 고유 식별자입니다.</param>
        /// <param name="builderFunc">액션을 생성하는 함수 포인터입니다.</param>
        /// <returns>등록이 성공하면 true를, 실패하면 false를 반환합니다.</returns>
        template <typename T> requires std::is_base_of<Action, T>::value
        static bool RegisterActionBuilder();

    private:
        inline static std::unordered_map<Monster::ActionID, ActionBuildFunc> _actionBuilderTable;

    };
    template <typename T> requires std::is_base_of<Action, T>::value
    inline bool ActionFactory::RegisterActionBuilder()
    {
        std::function<Action*()> factoryFunc = []() { return new T(); };
        int ID = T::ID;
        if (false == _actionBuilderTable.contains(ID))
        {
            _actionBuilderTable[ID] = factoryFunc;
            return true;
        }
        return false;
    }
}; // namespace Monster
