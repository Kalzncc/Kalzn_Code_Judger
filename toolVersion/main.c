#include "judger_config.h"
#include "cjson/cJSON.h"
#include "loglib.h"
#include "judgerlib.h"


/*
gcc  -D__LINUX_RUN judger_config.h securitylib.h securitylib.c  compilerlib.h compilerlib.c  judger_config.c loglib.h loglib.c cjson/cJSON.h cjson/cJSON.c killerlib.h killerlib.c boxlib.h boxlib.c judgerlib.h judgerlib.c matcherlib.h matcherlib.c main.c -o main.exe -lseccomp
main.exe json/JudgeConfig.json
*/

JudgeConfig judgeConfig;
JudgerConfig judgerConfig;

cJSON *cjson;
cJSON *judgerConfigJson;
cJSON *taskConfigJson;
cJSON *translatorJson;
cJSON *dataJson;
cJSON *temps[128];
int dataCnt;
int compilerOptionCnt;
int interpreterOptionCnt;


void printnum(const char * str, long long value) {
    printf("%s : %lld\n", str, value);
}
void prints(const char * str, const char* value) {
    printf("%s : %s\n", str, value);
}
void print() {
    printnum("maxCharBuffer", judgerConfig.maxCharBuffer);
    printnum("maxSPJTime", judgerConfig.maxSPJTime);
    printnum("maxSPJMemory", judgerConfig.maxSPJMemory);
    printnum("taskID", judgeConfig.taskID);
    printnum("uid", judgeConfig.uid);
    printnum("gid", judgeConfig.gid);
    printnum("judgeMode", judgeConfig.judgeMode);
    printnum("iOMode", judgeConfig.iOMode);
    printnum("strictMode", judgeConfig.strictMode);
    prints("workSpacePath", judgeConfig.workSpacePath);
    prints("logPath", judgeConfig.logPath);
    printnum("isSPJ", judgeConfig.isSPJ);
    if (judgeConfig.isSPJ) {
        prints("spjExePath", judgeConfig.sPJPath);
        prints("spjExeName", judgeConfig.sPJName);
    }
    printnum("translator-mode", judgeConfig.translator.mode);
    if (judgeConfig.translator.mode != INTERPRETER_MOD) {
        prints("compilerPath", judgeConfig.translator.compilerPath);
        int i;
        printf("compilerOption : ");
        for (i = 0; i < compilerOptionCnt; i++) {
            printf("%s ", judgeConfig.translator.compilerOptions[i]);
        }
        puts("");
        prints("compilerProductName", judgeConfig.translator.compilerProductName);
    }
    if (judgeConfig.translator.mode != COMPILER_MOD) {
        prints("interpreterPath", judgeConfig.translator.interpreterPath);
        int i;
        printf("interpreterOptions : ");
        for (i = 0; i < interpreterOptionCnt; i++) {
            printf("%s ", judgeConfig.translator.interpreterOptions[i]);
        }
        puts("");
    }
    int i;
    for (i = 0; i < dataCnt; i++) {
        printf("data %d ------- : \n", i);
        prints("inputData", judgeConfig.inputData[i]);
        prints("outputData", judgeConfig.outputData[i]);
        prints("stdAnswer", judgeConfig.stdAnswer[i]);
        printnum("maxCPUTime", judgeConfig.maxCPUTime[i]);
        printnum("maxMemory", judgeConfig.maxMemory[i]);
        printnum("maxStack", judgeConfig.maxStack[i]);
    }
}

void errorExit() {
    freeJudgeConfig(judgeConfig);
    freeJudgerConfig(judgerConfig);
    if (cjson != NULL) cJSON_Delete(cjson);
    if (judgerConfigJson!=NULL) cJSON_Delete(judgerConfigJson);
    if (taskConfigJson !=NULL) cJSON_Delete(taskConfigJson);
    if (translatorJson != NULL) cJSON_Delete(translatorJson);
    if (dataJson != NULL) cJSON_Delete(dataJson);
    cJSON *i;
    for (i = temps[0]; i != NULL; i++) {
        cJSON_Delete(i);
    }
    
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]) {
    cJSON * temp = temps[0];
    printf("sdd");
    char *logPath = DEFAULT_LOG_PATH;
    FILE * jsonFile;
    if (argc != 2 && argc != 3) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Input judge config invalid.");
        errorExit();
    } 
    if (argc == 3) {
        logPath = argv[2];
    }
    if ( (jsonFile = fopen(argv[1], "r") ) == NULL) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Can't open the json file.");
        errorExit();
    }
    char ch; int cnt = 0; char jsonStr[MAX_JSON_CHAR_NUM+2];

    while( (ch = fgetc(jsonFile))!=EOF && cnt <= MAX_JSON_CHAR_NUM)  {
        jsonStr[cnt++] = ch;
    }
    if (cnt > MAX_JSON_CHAR_NUM) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Json file too large.");
        errorExit();
    }
    jsonStr[cnt] = '\0';
    cjson = cJSON_Parse(jsonStr);
    judgerConfigJson = cJSON_GetObjectItem(cjson, "Judger");
    if (judgerConfigJson == NULL) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Judger config not is found.");
        errorExit();
    }
    taskConfigJson = cJSON_GetObjectItem(cjson, "Task");
    if (taskConfigJson == NULL) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Task config not is found.");
        errorExit();
    }
    translatorJson = cJSON_GetObjectItem(taskConfigJson, "translator");
    if (translatorJson == NULL) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "translator config not is found.");
        errorExit();
    }
    
    dataJson = cJSON_GetObjectItem(taskConfigJson, "data");
    if (dataJson == NULL) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "data config not is found.");
        errorExit();
    }
    if ( ( temp = cJSON_GetObjectItem(judgerConfigJson, "maxCharBuffer") ) == NULL || !cJSON_IsString(temp)) {
        
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : maxCharBuffer");
         
        errorExit();
    }
    
    judgerConfig.maxCharBuffer = atol(temp->valuestring);
     
    if (judgerConfig.maxCharBuffer < 1) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : maxCharBuffer");
        errorExit();
    }
    temp++;
    if ( ( temp = cJSON_GetObjectItem(judgerConfigJson, "maxSPJTime") ) == NULL || !cJSON_IsNumber(temp)) {
        judgerConfig.maxSPJTime = MAX_SPJ_TIME_LIMIT;
        
    }  else {
        judgerConfig.maxSPJTime = temp->valueint;
    }
    if (judgerConfig.maxSPJTime < 1) {
        judgerConfig.maxSPJTime = MAX_SPJ_TIME_LIMIT;
    }

    temp++;
    if ( ( temp = cJSON_GetObjectItem(judgerConfigJson, "maxSPJMemory") ) == NULL || !cJSON_IsString(temp)) {
        judgerConfig.maxSPJTime = MAX_SPJ_MEMORY_LIMIT;
    }
    judgerConfig.maxSPJMemory = atol(temp->valuestring);
    if (judgerConfig.maxSPJMemory < 1) {
        judgerConfig.maxSPJTime = MAX_SPJ_MEMORY_LIMIT;
    }

    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "taskID") ) == NULL || !cJSON_IsString(temp)) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : taskID");
         
        errorExit();
    }
    
    judgeConfig.taskID = atol(temp->valuestring);
     
    if (judgeConfig.taskID < 1) {
         logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : taskID");
        errorExit();
    }

    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "uid") ) == NULL || !cJSON_IsNumber(temp)) {
        judgeConfig.uid = DEFAULT_UID;
    } else {
        judgeConfig.uid = temp->valueint;
    }
     
    
    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "gid") ) == NULL || !cJSON_IsNumber(temp)) {
        judgeConfig.gid = DEFAULT_UID;
    } else {
        judgeConfig.gid = temp->valueint;
    }
     
    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "judgeMode") ) == NULL || !cJSON_IsNumber(temp)) {
        judgeConfig.judgeMode = 0;
    } else {
        judgeConfig.judgeMode = temp->valueint;
    }
     
    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "iOMode") ) == NULL || !cJSON_IsNumber(temp)) {
        judgeConfig.iOMode = 0;
    } else {
        judgeConfig.iOMode = temp->valueint;
    }
     
    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "strictMode") ) == NULL || !cJSON_IsBool(temp)) {
        judgeConfig.strictMode = NOT_STRICT_MODE;
    } else {
        judgeConfig.strictMode = temp->valueint;
    }
     
    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "workSpacePath") ) == NULL || !cJSON_IsString(temp)) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : workSpacePath");
         
        errorExit();
    } else {
        judgeConfig.workSpacePath = temp->valuestring;
         
    }


    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "logPath") ) == NULL || !cJSON_IsString(temp)) {
        judgeConfig.logPath = "log.txt";
    } else {
        judgeConfig.logPath = temp->valuestring;
    }
     
    temp++;
    if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "isSPJ") ) == NULL || !cJSON_IsBool(temp)) {
        judgeConfig.isSPJ = 0;
    } else {
        judgeConfig.isSPJ = temp->valueint;
    }
     

    if (judgeConfig.isSPJ != 0) {
        temp++;
        if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "spjExePath") ) == NULL || !cJSON_IsString(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : spjExePath");
             
        errorExit();
        } else {
            judgeConfig.sPJPath = temp->valuestring;
             
        }

        temp++;
        if ( ( temp = cJSON_GetObjectItem(taskConfigJson, "spjExeName") ) == NULL || !cJSON_IsString(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : spjExeName");
             
            errorExit();
        } else {
            judgeConfig.sPJName = temp->valuestring;
             
        }
    }

    temp++;
    if ( ( temp = cJSON_GetObjectItem(translatorJson, "mode") ) == NULL || !cJSON_IsNumber(temp)) {
        judgeConfig.translator.mode = COMPILER_MOD;
    } else {
        judgeConfig.translator.mode = temp->valueint;
    }
     

    if (judgeConfig.translator.mode != INTERPRETER_MOD) {
        temp++;
        if ( ( temp = cJSON_GetObjectItem(translatorJson, "compilerPath") ) == NULL || !cJSON_IsString(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : compilerPath");
             
            errorExit();
        } else {
            judgeConfig.translator.compilerPath = temp->valuestring;
             
        }

        temp++;
        if ( ( temp = cJSON_GetObjectItem(translatorJson, "compilerOptions") ) == NULL || !cJSON_IsArray(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : compilerOptions");
             
            errorExit();
        } else {
            compilerOptionCnt = cJSON_GetArraySize(temp);
            judgeConfig.translator.compilerOptions = (char **)malloc(sizeof(char*) * compilerOptionCnt+1);
            judgeConfig.translator.compilerOptions[compilerOptionCnt] = NULL;
            int i;
            for (i = 0; i < compilerOptionCnt; i++) {
                if ( !cJSON_IsString(cJSON_GetArrayItem(temp, i)) ) {
                    logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : compilerOptions");
                     
                    errorExit();
                }
                judgeConfig.translator.compilerOptions[i] = cJSON_GetArrayItem(temp, i)->valuestring;
            }
             
        }

        temp++;
        if ( ( temp = cJSON_GetObjectItem(translatorJson, "compilerProductName") ) == NULL || !cJSON_IsString(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : compilerProductName");
             
            errorExit();
        } else {
            judgeConfig.translator.compilerProductName = temp->valuestring;
             
        }
    }
    
    if (judgeConfig.translator.mode != COMPILER_MOD) {
        temp++;
        if ( ( temp = cJSON_GetObjectItem(translatorJson, "interpreterPath") ) == NULL || !cJSON_IsString(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : interpreterPath");
             
            errorExit();
        } else {
            judgeConfig.translator.interpreterPath = temp->valuestring;
             
        }

        temp++;
        if ( ( temp = cJSON_GetObjectItem(translatorJson, "interpreterOptions") ) == NULL || !cJSON_IsArray(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : interpreterOptions");
             
            errorExit();
        } else {
            interpreterOptionCnt = cJSON_GetArraySize(temp);
            judgeConfig.translator.interpreterOptions = (char **)malloc(sizeof(char*) * interpreterOptionCnt+1);
            judgeConfig.translator.interpreterOptions[interpreterOptionCnt] = NULL;
            int i;
            for (i = 0; i < interpreterOptionCnt; i++) {
                if ( !cJSON_IsString(cJSON_GetArrayItem(temp, i)) ) {
                    logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : interpreterOptions");
                     
                    errorExit();
                }
                judgeConfig.translator.interpreterOptions[i] = cJSON_GetArrayItem(temp, i)->valuestring;
            }
             
        }
    }

    if (!cJSON_IsArray(dataJson)) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
        errorExit();
    }

    dataCnt = cJSON_GetArraySize(dataJson);
    if (dataCnt > MAX_DATA_CASE_NUMBER) {
        logSystemErrorWithMessage(logPath, INVALID_JUDGE_CONFIG, "Data case too many");
        errorExit();
    }
    if (dataCnt < 1) {
        logSystemErrorWithMessage(logPath, INVALID_JUDGE_CONFIG, "Must to have least one data case");
        errorExit();
    }
    judgeConfig.inputData = (char **) malloc(sizeof(char *) * dataCnt);
    judgeConfig.outputData = (char **) malloc(sizeof(char *) * dataCnt);
    judgeConfig.stdAnswer = (char **) malloc(sizeof(char *) * dataCnt);
    judgeConfig.maxCPUTime = (int *) malloc(sizeof(int) * dataCnt);
    judgeConfig.maxMemory = (long long *) malloc(sizeof(long long) * dataCnt);
    judgeConfig.maxStack = (int *) malloc(sizeof(int) * dataCnt);
    int i;
    
    for (i = 0; i < dataCnt; i++) {

        temp++;
        temp = cJSON_GetArrayItem(dataJson, i);
        char *inputData, *outputData, *stdAnswer;
        int maxCPUTime, maxStack;
        long long maxMemory;

        cJSON* atr;
        if ((atr = cJSON_GetObjectItem(temp, "inputData")) == NULL || !cJSON_IsString(atr)) {
            cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }
        inputData = atr->valuestring;
        
        
        if ((atr = cJSON_GetObjectItem(temp, "outputData")) == NULL || !cJSON_IsString(atr)) {
             cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }
        outputData = atr->valuestring;
           
        
        if ((atr = cJSON_GetObjectItem(temp, "stdAnswer")) == NULL || !cJSON_IsString(atr)) {   
            cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }
        stdAnswer = atr->valuestring;
           

        if ((atr = cJSON_GetObjectItem(temp, "maxMemory")) == NULL || !cJSON_IsString(atr)) {
            cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }
        maxMemory = atol(atr->valuestring);
        if (maxMemory < 1){
            cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }

        if ((atr = cJSON_GetObjectItem(temp, "maxCPUTime")) == NULL || !cJSON_IsNumber(atr)) {
            cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }
        maxCPUTime = atr->valueint;
        if (maxCPUTime < 1){
            cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }

        if ((atr = cJSON_GetObjectItem(temp, "maxStack")) == NULL || !cJSON_IsNumber(atr)) {
            cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }
        maxStack = atr->valueint;
        if (maxStack < 1){
            cJSON_Delete(atr);
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
            errorExit();
        }
        judgeConfig.inputData[i] = inputData;
        judgeConfig.outputData[i] = outputData;
        judgeConfig.stdAnswer[i] = stdAnswer;
        judgeConfig.maxCPUTime[i] = maxCPUTime;
        judgeConfig.maxMemory[i] = maxMemory;
        judgeConfig.maxStack[i] = maxStack;
        cJSON_Delete(atr);
        
    }
    if (judgeConfig.translator.mode == COMPILER_MOD) {
        judgeConfig.translator.interpreterPath = judgeConfig.translator.compilerProductName;
        judgeConfig.translator.interpreterOptions = (char **) malloc(sizeof(char*));
        judgeConfig.translator.interpreterOptions[0] = NULL;
    }
    
    judgeConfig.caseNumber = dataCnt;
    print();


    
    RunConfig * result = judge(&judgerConfig, &judgeConfig);
    
    
    

    return 0;
}