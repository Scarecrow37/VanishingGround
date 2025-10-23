#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class ChainDeffence : public Token
    {
        void OnRoundStart(CharacterBase* owner) override;
        void OnPostPlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                            int& damage) override;
        void OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                           int& damage) override;

    protected:
        virtual int GetDeffenceChainCount() const = 0;
    };

    class ChainDeffence2 : public ChainDeffence
    {
        TOKEN_DATA(205030)
        inline int GetDeffenceChainCount() const override { return 2; }
    };
    class ChainDeffence3 : public ChainDeffence
    {
        TOKEN_DATA(205031)
        inline int GetDeffenceChainCount() const override { return 3; }
    };
    class ChainDeffence4 : public ChainDeffence
    {
        TOKEN_DATA(205032)
        inline int GetDeffenceChainCount() const override { return 4; }
    };
    class ChainDeffence5 : public ChainDeffence
    {
        TOKEN_DATA(205033)
        inline int GetDeffenceChainCount() const override { return 5; }
    };
    class ChainDeffence6 : public ChainDeffence
    {
        TOKEN_DATA(205034)
        inline int GetDeffenceChainCount() const override { return 6; }
    };
    class ChainDeffence7 : public ChainDeffence
    {
        TOKEN_DATA(205035)
        inline int GetDeffenceChainCount() const override { return 7; }
    };
    class ChainDeffence8 : public ChainDeffence
    {
        TOKEN_DATA(205036)
        inline int GetDeffenceChainCount() const override { return 8; }
    };
}
