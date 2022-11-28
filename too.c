#include "redismodule.h"
#include <stdlib.h>

double CalcJaccard(RedisModuleCtx *ctx, RedisModuleString *class, RedisModuleString *user, RedisModuleString *other);

int TooLike_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 4) return RedisModule_WrongArity(ctx);

    RedisModule_AutoMemory(ctx);

    RedisModuleCallReply *reply;

    RedisModuleString *likeskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", RedisModule_StringPtrLen(argv[1], 0), RedisModule_StringPtrLen(argv[3], 0));
    reply = RedisModule_Call(ctx, "SADD", "ss", likeskey, argv[2]);

    RedisModuleString *likerskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likers", RedisModule_StringPtrLen(argv[1], 0), RedisModule_StringPtrLen(argv[2], 0));
    reply = RedisModule_Call(ctx, "SADD", "ss", likerskey, argv[3]);

    RedisModule_ReplyWithLongLong(ctx, 1);
    return REDISMODULE_OK;
}

int TooUnlike_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 4) return RedisModule_WrongArity(ctx);

    RedisModule_AutoMemory(ctx);

    RedisModuleCallReply *reply;

    RedisModuleString *likeskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", RedisModule_StringPtrLen(argv[1], 0), RedisModule_StringPtrLen(argv[3], 0));
    reply = RedisModule_Call(ctx, "SREM", "ss", likeskey, argv[2]);

    RedisModuleString *likerskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likers", RedisModule_StringPtrLen(argv[1], 0), RedisModule_StringPtrLen(argv[2], 0));
    reply = RedisModule_Call(ctx, "SREM", "ss", likerskey, argv[3]);

    RedisModule_ReplyWithLongLong(ctx, 1);
    return REDISMODULE_OK;
}

int TooDislike_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 4) return RedisModule_WrongArity(ctx);

    RedisModule_AutoMemory(ctx);

    RedisModuleCallReply *reply;

    RedisModuleString *dislikeskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikes", RedisModule_StringPtrLen(argv[1], 0), RedisModule_StringPtrLen(argv[3], 0));
    reply = RedisModule_Call(ctx, "SADD", "ss", dislikeskey, argv[2]);

    RedisModuleString *dislikerskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikers", RedisModule_StringPtrLen(argv[1], 0), RedisModule_StringPtrLen(argv[2], 0));
    reply = RedisModule_Call(ctx, "SADD", "ss", dislikerskey, argv[3]);

    RedisModule_ReplyWithLongLong(ctx, 1);
    return REDISMODULE_OK;
}

int TooUndislike_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 4) return RedisModule_WrongArity(ctx);

    RedisModule_AutoMemory(ctx);

    RedisModuleCallReply *reply;

    RedisModuleString *dislikeskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikes", RedisModule_StringPtrLen(argv[1], 0), RedisModule_StringPtrLen(argv[3], 0));
    reply = RedisModule_Call(ctx, "SREM", "ss", dislikeskey, argv[2]);

    RedisModuleString *dislikerskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikers", RedisModule_StringPtrLen(argv[1], 0), RedisModule_StringPtrLen(argv[2], 0));
    reply = RedisModule_Call(ctx, "SREM", "ss", dislikerskey, argv[3]);

    RedisModule_ReplyWithLongLong(ctx, 1);
    return REDISMODULE_OK;
}

int TooRefresh_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 3) return RedisModule_WrongArity(ctx);

    RedisModule_AutoMemory(ctx);

    RedisModuleString *class = argv[1];
    RedisModuleString *user = argv[2];

    char *classptr = RedisModule_StringPtrLen(class, 0);
    char *userptr = RedisModule_StringPtrLen(user, 0);

    RedisModuleCallReply *reply;

    RedisModuleString *likeskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", classptr, userptr);
    RedisModuleString *dislikeskey = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikes", classptr, userptr);
    reply = RedisModule_Call(ctx, "SUNION", "ss", likeskey, dislikeskey);

    size_t n = RedisModule_CallReplyLength(reply);
    RedisModuleString **args = RedisModule_Alloc(sizeof(RedisModuleString*) * n * 2);
    for (int i = 0; i < n; i++) {
        args[i*2] = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likers", classptr, RedisModule_StringPtrLen(RedisModule_CreateStringFromCallReply(RedisModule_CallReplyArrayElement(reply, i)), 0));
        args[i*2+1] = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikers", classptr, RedisModule_StringPtrLen(RedisModule_CreateStringFromCallReply(RedisModule_CallReplyArrayElement(reply, i)), 0));
    }

    reply = RedisModule_Call(ctx, "SUNION", "v", args, n*2);
    RedisModule_Free(args);

    n = RedisModule_CallReplyLength(reply);
    for (int i = 0; i < n; i++) {
        RedisModuleString *other = RedisModule_CreateStringFromCallReply(RedisModule_CallReplyArrayElement(reply, i));
        if (RedisModule_StringCompare(user, other) == 0) {
            continue;
        }

        double jaccard = CalcJaccard(ctx, class, user, other);
        RedisModuleCallReply *zaddreply;
        zaddreply = RedisModule_Call(ctx, "ZADD", "sss", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:similars", classptr, userptr), RedisModule_CreateStringFromDouble(ctx, jaccard), other);
    }

    reply = RedisModule_Call(ctx, "ZRANGE", "sccc", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:similars", classptr, userptr), "0", "-1", "REV");
    
    n = RedisModule_CallReplyLength(reply);
    args = RedisModule_Alloc(sizeof(RedisModuleString*) * n);
    for (int i = 0; i < n; i++) {
        args[i] = RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", classptr, RedisModule_StringPtrLen(RedisModule_CreateStringFromCallReply(RedisModule_CallReplyArrayElement(reply, i)), 0));
    }

    reply = RedisModule_Call(ctx, "SUNIONSTORE", "sv", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:tmp:options", classptr, userptr), args, n);
    RedisModule_Free(args);

    reply = RedisModule_Call(ctx, "SDIFF", "sss", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:tmp:options", classptr, userptr), RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", classptr, userptr), RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikes", classptr, userptr));

    n = RedisModule_CallReplyLength(reply);
    for (int i = 0; i < n; i++) {
        RedisModuleString *item = RedisModule_CreateStringFromCallReply(RedisModule_CallReplyArrayElement(reply, i));
        char *itemptr = RedisModule_StringPtrLen(item, 0);

        double score = 0;
        double total = 0;

        RedisModuleCallReply *smembersreply;
        smembersreply = RedisModule_Call(ctx, "SMEMBERS", "s", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likers", classptr, itemptr));

        size_t nmembers = RedisModule_CallReplyLength(smembersreply);
        for (int j = 0; j < nmembers; j++) {
            RedisModuleString *liker = RedisModule_CreateStringFromCallReply(RedisModule_CallReplyArrayElement(smembersreply, j));

            RedisModuleCallReply *zscorereply = RedisModule_Call(ctx, "ZSCORE", "ss", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:similars", classptr, userptr), liker);
            RedisModuleString *scorestr = RedisModule_CreateStringFromCallReply(zscorereply);
            char *scoreptr = RedisModule_StringPtrLen(scorestr, 0);
            score += atof(scoreptr);
            total++;

        }

        smembersreply = RedisModule_Call(ctx, "SMEMBERS", "s", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikers", classptr, itemptr));

        nmembers = RedisModule_CallReplyLength(smembersreply);
        for (int j = 0; j < nmembers; j++) {
            RedisModuleString *disliker = RedisModule_CreateStringFromCallReply(RedisModule_CallReplyArrayElement(smembersreply, j));

            RedisModuleCallReply *zscorereply = RedisModule_Call(ctx, "ZSCORE", "ss", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:similars", classptr, userptr), disliker);
            RedisModuleString *scorestr = RedisModule_CreateStringFromCallReply(zscorereply);
            char *scoreptr = RedisModule_StringPtrLen(scorestr, 0);
            score += atof(scoreptr);
            total++;
            
        }

        score /= total;

        RedisModuleCallReply *zaddreply = RedisModule_Call(ctx, "ZADD", "sss", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:newsuggestions", classptr, userptr), RedisModule_CreateStringFromDouble(ctx, score), item);

    }

    reply = RedisModule_Call(ctx, "RENAME", "ss", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:newsuggestions", classptr, userptr), RedisModule_CreateStringPrintf(ctx, "too:%s:%s:suggestions", classptr, userptr));

    reply = RedisModule_Call(ctx, "DEL", "s", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:tmp:options", classptr, userptr));

    RedisModule_ReplyWithLongLong(ctx, 1);
    return REDISMODULE_OK;
}

int TooSuggest_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (argc != 3) return RedisModule_WrongArity(ctx);

    RedisModule_AutoMemory(ctx);

    RedisModuleString *class = argv[1];
    RedisModuleString *user = argv[2];

    char *classptr = RedisModule_StringPtrLen(class, 0);
    char *userptr = RedisModule_StringPtrLen(user, 0);

    RedisModuleCallReply *reply = RedisModule_Call(ctx, "ZRANGE", "sccc", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:suggestions", classptr, userptr), "0", "-1", "REV");
    RedisModule_ReplyWithCallReply(ctx, reply);
    return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (RedisModule_Init(ctx, "too", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "too.like", TooLike_RedisCommand, "write deny-oom fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "too.unlike", TooUnlike_RedisCommand, "write deny-oom fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "too.dislike", TooDislike_RedisCommand, "write deny-oom fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "too.undislike", TooUndislike_RedisCommand, "write deny-oom fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "too.refresh", TooRefresh_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "too.suggest", TooSuggest_RedisCommand, "readonly fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    return REDISMODULE_OK;
}

double CalcJaccard(RedisModuleCtx *ctx, RedisModuleString *class, RedisModuleString *user, RedisModuleString *other) {
    RedisModule_AutoMemory(ctx);

    char *classptr = RedisModule_StringPtrLen(class, 0);
    char *userptr = RedisModule_StringPtrLen(user, 0);
    char *otherptr = RedisModule_StringPtrLen(other, 0);

    RedisModuleCallReply *reply;

    reply = RedisModule_Call(ctx, "SINTERCARD", "css", "2", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", classptr, userptr), RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", classptr, otherptr));
    long long likes = RedisModule_CallReplyInteger(reply);

    reply = RedisModule_Call(ctx, "SINTERCARD", "css", "2", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikes", classptr, userptr), RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikes", classptr, otherptr));
    long long dislikes = RedisModule_CallReplyInteger(reply);

    reply = RedisModule_Call(ctx, "SINTERCARD", "css", "2", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", classptr, userptr), RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikes", classptr, otherptr));
    long long antilikes = RedisModule_CallReplyInteger(reply);

    reply = RedisModule_Call(ctx, "SINTERCARD", "css", "2", RedisModule_CreateStringPrintf(ctx, "too:%s:%s:dislikes", classptr, userptr), RedisModule_CreateStringPrintf(ctx, "too:%s:%s:likes", classptr, otherptr));
    long long antidislikes = RedisModule_CallReplyInteger(reply);

    return (double)(likes + dislikes - antilikes - antidislikes) / (double)(likes + dislikes + antilikes + antidislikes);
}
