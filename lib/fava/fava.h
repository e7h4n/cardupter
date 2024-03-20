#pragma once

struct FavaLedgerStat
{
    float creditCardBalance;
    float cashBalance;
    float xirr;
};

FavaLedgerStat getFavaLedgerStat();
