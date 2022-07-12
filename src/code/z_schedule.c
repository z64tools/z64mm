#include "global.h"

#define SCHEDULE_CALC_TIME(hour, minute, dest, temp) \
    (temp) = (hour)*60.0f;                           \
    (temp) += (minute);                              \
    (dest) = (temp) * (0x10000 / 60 / 24.0f);        \
    (dest) = SCHEDULE_CONVERT_TIME(dest);

s32 Schedule_CheckFlagS(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckFlagS* cmd = (ScheduleCmdCheckFlagS*)*script;
    u16 flag = (cmd->flagByte << 8) | cmd->flagMask;

    if (gSaveContext.save.weekEventReg[flag >> 8] & (flag & 0xFF)) {
        *script += cmd->offset;
    }

    return false;
}

s32 Schedule_CheckFlagL(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckFlagL* cmd = (ScheduleCmdCheckFlagL*)*script;
    u16 flag = (cmd->flagByte << 8) | cmd->flagMask;

    if (gSaveContext.save.weekEventReg[flag >> 8] & (flag & 0xFF)) {
        *script += (s16)((cmd->offsetH << 8) | cmd->offsetL);
    }

    return false;
}

s32 Schedule_CheckTimeRangeS(PlayState* play, u8** script, ScheduleResult* result) {
    s32 inRange = false;
    ScheduleCmdCheckTimeRangeS* cmd = (ScheduleCmdCheckTimeRangeS*)*script;
    f32 f;
    u16 start;
    u16 end;
    u16 now;

    SCHEDULE_CALC_TIME(cmd->startHr, cmd->startMin, start, f);

    SCHEDULE_CALC_TIME(cmd->endHr, cmd->endMin, end, f);
    end--;

    now = SCHEDULE_TIME_NOW;

    if ((start <= now) && (now <= end)) {
        inRange = true;
    }

    if (inRange == true) {
        *script += cmd->offset;
    }

    return false;
}

s32 Schedule_CheckTimeRangeL(PlayState* play, u8** script, ScheduleResult* result) {
    s32 inRange = false;
    ScheduleCmdCheckTimeRangeL* cmd = (ScheduleCmdCheckTimeRangeL*)*script;
    f32 f;
    u16 start;
    u16 end;
    u16 now;

    SCHEDULE_CALC_TIME(cmd->startHr, cmd->startMin, start, f);

    SCHEDULE_CALC_TIME(cmd->endHr, cmd->endMin, end, f);
    end--;

    now = SCHEDULE_TIME_NOW;

    if ((start <= now) && (now <= end)) {
        inRange = true;
    }

    if (inRange == true) {
        *script += (s16)((cmd->offsetH << 8) | cmd->offsetL);
    }

    return false;
}

s32 Schedule_ReturnValueL(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdReturnValueL* cmd = (ScheduleCmdReturnValueL*)*script;

    //! @bug result is a u8, value is truncated
    result->result = (cmd->retH << 8) | cmd->retL;
    result->hasResult = true;

    return true;
}

s32 Schedule_ReturnNone(PlayState* play, u8** script, ScheduleResult* result) {
    result->hasResult = false;

    return true;
}

s32 Schedule_ReturnEmpty(PlayState* play, u8** script, ScheduleResult* result) {
    result->hasResult = true;

    return true;
}

s32 Schedule_Nop(PlayState* play, u8** script, ScheduleResult* result) {
    return false;
}

s32 Schedule_CheckMiscS(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckMiscS* cmd = (ScheduleCmdCheckMiscS*)*script;

    if (((cmd->which == SCHEDULE_CHECK_MISC_ROOM_KEY) && (INV_CONTENT(ITEM_ROOM_KEY) == ITEM_ROOM_KEY)) ||
        ((cmd->which == SCHEDULE_CHECK_MISC_LETTER_TO_KAFEI) &&
         (INV_CONTENT(ITEM_LETTER_TO_KAFEI) == ITEM_LETTER_TO_KAFEI)) ||
        ((cmd->which == SCHEDULE_CHECK_MISC_MASK_ROMANI) && (Player_GetMask(play) == PLAYER_MASK_ROMANI))) {
        *script += cmd->offset;
    }

    return false;
}

s32 Schedule_ReturnValueS(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdReturnValueS* cmd = (ScheduleCmdReturnValueS*)*script;

    result->result = cmd->result;
    result->hasResult = true;

    return true;
}

s32 Schedule_CheckNotInSceneS(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckNotInSceneS* cmd = (ScheduleCmdCheckNotInSceneS*)*script;
    s16 scene = (cmd->sceneH << 8) | cmd->sceneL;

    if (scene != play->sceneNum) {
        *script += cmd->offset;
    }

    return false;
}

s32 Schedule_CheckNotInSceneL(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckNotInSceneL* cmd = (ScheduleCmdCheckNotInSceneL*)*script;
    s16 scene = (cmd->sceneH << 8) | cmd->sceneL;

    if (scene != play->sceneNum) {
        *script = *script + (s16)((cmd->offsetH << 8) | cmd->offsetL);
    }

    return false;
}

s32 Schedule_CheckNotInDayS(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckNotInDayS* cmd = (ScheduleCmdCheckNotInDayS*)*script;
    s16 day = (cmd->dayH << 8) | cmd->dayL;

    if (day != (s16)gSaveContext.save.day) {
        *script += cmd->offset;
    }

    return false;
}

s32 Schedule_CheckNotInDayL(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckNotInDayL* cmd = (ScheduleCmdCheckNotInDayL*)*script;
    s16 day = (cmd->dayH << 8) | cmd->dayL;

    if (day != (s16)gSaveContext.save.day) {
        *script += (s16)((cmd->offsetH << 8) | cmd->offsetL);
    }

    return false;
}

s32 Schedule_ReturnTime(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdReturnTime* cmd = (ScheduleCmdReturnTime*)*script;
    f32 f;
    u16 time0;
    u16 time1;

    SCHEDULE_CALC_TIME(cmd->time0Hr, cmd->time0Min, time0, f);

    SCHEDULE_CALC_TIME(cmd->time1Hr, cmd->time1Min, time1, f);
    time1--;

    result->result = cmd->result;
    result->time0 = time0;
    result->time1 = time1;
    result->hasResult = true;

    return true;
}

s32 Schedule_CheckBeforeTimeS(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckBeforeTimeS* cmd = (ScheduleCmdCheckBeforeTimeS*)*script;
    f32 f;
    u16 testTime;
    u16 now;

    SCHEDULE_CALC_TIME(cmd->timeHr, cmd->timeMin, testTime, f);

    now = SCHEDULE_TIME_NOW;

    if (now < testTime) {
        *script += cmd->offset;
    }

    return false;
}

s32 Schedule_CheckBeforeTimeL(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdCheckBeforeTimeL* cmd = (ScheduleCmdCheckBeforeTimeL*)*script;
    f32 f;
    u16 testTime;
    u16 now;

    SCHEDULE_CALC_TIME(cmd->timeHr, cmd->timeMin, testTime, f);

    now = SCHEDULE_TIME_NOW;

    if (now < testTime) {
        *script += (s16)((cmd->offsetH << 8) | cmd->offsetL);
    }

    return false;
}

s32 Schedule_BranchS(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdBranchS* cmd = (ScheduleCmdBranchS*)*script;

    *script += cmd->offset;
    return false;
}

s32 Schedule_BranchL(PlayState* play, u8** script, ScheduleResult* result) {
    ScheduleCmdBranchL* cmd = (ScheduleCmdBranchL*)*script;

    *script += (s16)((cmd->offsetH << 8) | cmd->offsetL);
    return false;
}

static s32 (*sScheduleCmdFuncs[])(PlayState*, u8**, ScheduleResult*) = {
    Schedule_CheckFlagS,       Schedule_CheckFlagL,     Schedule_CheckTimeRangeS,  Schedule_CheckTimeRangeL,
    Schedule_ReturnValueL,     Schedule_ReturnNone,     Schedule_ReturnEmpty,      Schedule_Nop,
    Schedule_CheckMiscS,       Schedule_ReturnValueS,   Schedule_CheckNotInSceneS, Schedule_CheckNotInSceneL,
    Schedule_CheckNotInDayS,   Schedule_CheckNotInDayL, Schedule_ReturnTime,       Schedule_CheckBeforeTimeS,
    Schedule_CheckBeforeTimeL, Schedule_BranchS,        Schedule_BranchL,
};

static u8 sScheduleCmdSizes[] = {
    sizeof(ScheduleCmdCheckFlagS),
    sizeof(ScheduleCmdCheckFlagL),
    sizeof(ScheduleCmdCheckTimeRangeS),
    sizeof(ScheduleCmdCheckTimeRangeL),
    sizeof(ScheduleCmdReturnValueL),
    sizeof(ScheduleCmdBase),
    sizeof(ScheduleCmdBase),
    sizeof(ScheduleCmdNop),
    sizeof(ScheduleCmdCheckMiscS),
    sizeof(ScheduleCmdReturnValueS),
    sizeof(ScheduleCmdCheckNotInSceneS),
    sizeof(ScheduleCmdCheckNotInSceneL),
    sizeof(ScheduleCmdCheckNotInDayS),
    sizeof(ScheduleCmdCheckNotInDayL),
    sizeof(ScheduleCmdReturnTime),
    sizeof(ScheduleCmdCheckBeforeTimeS),
    sizeof(ScheduleCmdCheckBeforeTimeL),
    sizeof(ScheduleCmdBranchS),
    sizeof(ScheduleCmdBranchL),
};

s32 Schedule_RunScript(PlayState* play, u8* script, ScheduleResult* result) {
    u8 size;
    s32 stop;

    do {
        size = sScheduleCmdSizes[*script];
        stop = (*sScheduleCmdFuncs[*script])(play, &script, result);
        script += size;
    } while (!stop);

    return result->hasResult;
}