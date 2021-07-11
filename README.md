# Kalzn_Code_Judger
## 简介
这是一个使用c编写的online judge的评测机，将在完善优化后作为KalznOJ的评测沙盒。Judger每次从一个json文件中读入任务的配置信息。然后生成评测结果，在测试例中，结果将写入工作区目录下的result.txt文本文件中。在正式使用时可以将结果写入json。
## 准备
目前Judger仅在centOS7以及ubnutu20.04中测试过，理论上适用于所有支持seccomp沙盒机制的linux版本。在进行编译前，请确保已经安装了seccomp库。

seccomp库安装方法（以ubuntu为例）

```sh
sudo apt install libseccomp-dev libseccomp2 seccomp
```

## 编译代码
请连接seccomp库进行编译。在toolVersion目录下
```sh
make
```
makefile将会对代码进行编译，并将产物``kalznjudger``移动至``/usr/bin``目录下。目前版本均为测试版本，错误和调试信息将全部计入log并显示在屏幕上。

## 开始使用
### 开始使用
使用时，请输入
```sh
sudo kalznjudger <json file path> [log path]
```
其中如果省略``log path``，在json解析成功之前，log目录将默认设置为Judger同目录下的``log.log``，当json解析成功后，log路径将采用json配置中的log。这里不建议省略``log path``，建议指令中``log path``与json中配置的````log path````保持一致。执行``kalznjudger``时，应使用root或sudo。否则judger将会报错。Judger评测完成后，会在工作区的RESULT_FILE_PATH目录下生成结果json文件。
### 删除kalznjudger
删除``/usr/bin``目录中的kalznjudger即可。
```
sudo rm -f /usr/bin/kalznjudger
```
### JSON文件
这里使用一个JSON文件作为示例，数据项的意义及注意事项见注释。由于cJSON库对long long int 型变量不支持，这里long long int型变量用字符串表示。
```json
{
    "Judger": {  // 评测机相关配置

        "maxCharBuffer" : "10000000",  //字符串最大缓存 必选项

        "maxSPJTime" : 10000, // SPJ时限，默认为 MAX_SPJ_TIME_LIMIT

        "maxSPJMemory": "102400000" // SPJ内存限制 默认 MAX_SPJ_MEMORY_LIMIT

        
    },
    "Task" : { // 任务相关配置

        "taskID":"100031", // 任务ID号 必选项，务必从1开始。0视为无效ID号。

        "judgeMode": 0, // 评测模式 默认为SINGLE_RESULT_MODE 具体见下表格 

        "iOMode": 0,  // IO模式 默认为STD_IO 见下表

        "gid" : 65534, // gid 默认为DEFAULT_UID 评测时的进程的effective gid

        "uid" : 65534,// uid 默认为DEFAULT_UID 评测时的进程的effective uid

        "strictMode" : true, // 严格模式，默认为NOT_STRICT_MODE 见下表

        "workSpacePath":"/home/kalzn/Desktop/testworkspace/", 
        // 工作区目录 必选项 评测该任务的工作区目录，以下涉及到目录的配置，均必须是相对工作区的目录，或者绝对目录。

        "logPath":"/home/kalzn/Desktop/testworkspace/log.log", // log目录 必选项

        "isSPJ": true, // 是否启用SPJ，默认为false

        "spjExePath":"/home/kalzn/Desktop/testworkspace/spj", // SPJ目录，当isSPJ=true时，必填项

        "spjExeName":"spj", // SPJ可执行文件名字，当isSPJ=true时，必填项

        "translator": { // 翻译选项

            "mode": 2, // 翻译模式 必选项 见下表

            "compilerPath": "/usr/bin/javac", // 编译器路径 当翻译模式!=INTERPRETER_MODE时，必选项

            "compilerOptions":[ // 编译器选项 当翻译模式!=INTERPRETER_MODE时，必选项
                "javac", "Main.java"
            ],

            "compilerProductName":"Main.class",
            // 编译产物名 当翻译模式!=INTERPRETER_MODE时，必选项，Judger编译后，将确认工作区目录下是否生成了名为compilerProductName的文件，以此判定编译是否正确。

            "interpreterPath":"/usr/bin/java", // 解释器路径，当翻译模式!=COMPILER_MODE时 必选项。

            "interpreterOptions":[ // 解释器选项，当翻译模式!=COMPILER_MODE时 必选项。
                "java", "Main"
            ]

        },
        "data" : [ // 数据配置，这里数据的数量不超过MAX_DATA_CASE_NUMBER个
            {
                "inputData":"data/1.in", // 输入数据

                "outputData":"data/output.out", // 输出文件（一般所有数据填一个即可）

                "stdAnswer":"data/1.out", // 标准输出
                
                "maxCPUTime": 1000, //时限

                "maxMemory" : "102400000000", // 内存限制
                
                "maxStack" : 1024000 //Stack数量限制。
            },
            {
                "inputData":"data/2.in",
                "outputData":"data/output.out",
                "stdAnswer":"data/2.out",
                
                "maxCPUTime": 1000,
                "maxMemory" : "102400000000",
                "maxStack" : 1024000
            },
            {
                "inputData":"data/3.in",
                "outputData":"data/output.out",
                "stdAnswer":"data/3.out",
                
                "maxCPUTime": 1000,
                "maxMemory" : "102400000000",
                "maxStack" : 1024000
            },
            {
                "inputData":"data/4.in",
                "outputData":"data/output.out",
                "stdAnswer":"data/4.out",
                
                "maxCPUTime": 1000,
                "maxMemory" : "102400000000",
                "maxStack" : 1024000
            }
        ]
    }
}
```
### 一些配置选项的说明
<table>
    <tr>
        <th> 选项</th>
        <th> 取值</th>
        <th> 意义</th>
    </tr>
    <tr>
        <td rowspan='3'>
            Task.judgeMode
        </td>
    </tr>
    <tr>
        <td>
            SINGLE_RESULT_MODE(0)
        </td>
        <td>
            单一结果模式，且若某样例没有AC，之后的样例直接跳过
        </td>
    </tr>
    <tr>
        <td>
            POINTS_MODE(1)
        </td>
        <td>
            积分模式，返回每个样例的结果，即使某样例没有AC，也会接着评测后面的样例。
        </td>
    </tr>
    <tr>
        <td rowspan='3'>
            Task.iOMode
        </td>
    </tr>
    <tr>
        <td>
            STD_IO(0)
        </td>
        <td>
            评测时使用标准读入输出
        </td>
    </tr>
    <tr>
        <td>
            FILE_IO(1)
        </td>
        <td>
            评测时使用文件读入输出，文件路径为FILEIO_INPUT_PATH以及FILEIO_OUTPUT_PATH
        </td>
    </tr>
    <tr>
        <td rowspan='3'>
            Task.strictMode
        </td>
    </tr>
    <tr>
        <td>
            NOT_STRICT_MODE(0)
        </td>
        <td>
		 非严格模式：<br/>
1、对字符串进行变换，把答案和输出数据中的"\r\n"，用"\n"替代<br/>
2、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。<br/>
3、将答案和输出的所有连续空白字符都用一个空格替代。<br/>
4、如果此时输出和答案相等，则AC。<br/>
5、否则WA。<br/>
        </td>
    </tr>
    <tr>
        <td>
            STRICT_MODE(1)
        </td>
        <td>
           严格评测模式：<br/>
如果为true，则启用严格评测模式，在严格模式下，按照以下顺序比对答案和输出：<br/>
1、对字符串进行变换，把答案和输出数据中的"\r\n"，用"\n"替代<br/>
2、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。<br/>
3、如果输出和答案相等，则AC。<br/>
4、否则：将答案和输出的所有连续空白字符都用一个空格替代。<br/>
5、如果此时输出和答案相等，则PE。<br/>
6、否则WA。<br/>
        </td>
    </tr>
    <tr>
        <td rowspan='4'>
            Task.translator.mode
        </td>
    </tr>
    <tr>
        <td >
            COMPILER_MODE(0)
        </td>
        <td>
            待测代码是工作区下Main.x, x为后缀名<br/>
1、compiler mode, 编译模式<br/>
此模式下，judger先切换到work space目录下，执行 [compilerCmd] 指令，
这里规定产生物的名字应为compilerProductName，此后judger将检测目录下是否成功生成名字为
compilerProductName的文件，以判定编译是否成功。如果失败则报告编译失败，否则执行runner开始评测。
这里runner经初始化，会直接将进程execute到编译后的产物。<br/>
        </td>
    </tr>
    <tr>
        <td>
            INTERPRETER_MODE(1)
        </td>
        <td>
        2、interpreter mode 解释模式<br/>
此模式下，judger先切换到work space目录下，直接开始执行runner，在runner初始化成功后，将会执行语句
[interpreterCmd] ,启用解释器运行代码，报错退出将按照runner的自行处理。
        </td>
    </tr>
    <tr>
        <td>
            COMPILER_INTERPRETER_MODE(2)
        </td>
        <td>
        3、compiler interpreter mode 编译解释模式<br/>
此模式下，judger先切换到work space目录下，执行 [compilerCmd] 指令
这里规定产生物的名字应为compilerProductName，此后judger将检测目录下是否成功生成名字为
compilerProductName的文件，以判定编译是否成功。如果失败则报告编译失败，否则执行runner开始评测。
在runner初始化成功后，将会执行语句 [interpreterCmd], 启用解释器运行代码，报错退出将按照runner的自行处理。<br/>
不过要明确的是，此部分的配置应该由系统的部署人员完成，此部分的配置一般不会修改
除非要改动OJ的编译指令，或者添加、删减某种语言。<br/>
        </td>
    </tr>
</table>

编译和解释过程中产生的输出信息会分别记录到COMPILER_INFO_PATH 和 INTERPRETER_INFO_PATH下，它将和评测结果文件，一同交由高层的评测队列管理模块回收处理。<br/>
如果出现某种语言无法适配的情况，就劳烦自行编写了。这里推荐在judger.h中声明专用的编译流程函数，在box初始化后调用。目前来说，常规的语言都可以适配。<br/>

## 工作区
工作区是Judger在评测时的工作目录，与评测有关的数据、待测源程序等最好都在此目录下，Judger所生成的编译器和解释器信息，以及最终结果都将会存储在此。当高层管理程序回收时，应该从工作区目录回收评测结果，并做好工作区的清理。

## 评测结果
| 结果名 | 结果代码 | 说明 |
| --- | --- | --- |
|ACCEPTED|0|该测试点通过了测试|
|WRONG_ANSWER|1|该测试点输出与标准答案不一致，或spj任何该待测程序答案错误|
|TIME_LIMIT_EXCEEDED|2|待测程序没有在时间限制内给出答案|
|MEMORY_LIMIT_EXCEEDED|3|待测程序没有在内存限制内给出答案|
|RUNTIME_ERROR|4|待测程序崩溃|
|PRESENTATION_ERROR|5|待测程序格式错误，此结果只在严格模式生效|
|OUTPUT_LIMIT_EXCEEDED|6|待测程序输出字符数大于maxCharBuffer|
|COMPILE_ERROR|7|待测程序没有通过编译|
|SKIP|8|跳过，当评测模式为SINGLE_RESULT_MODE时，如果一个测试点不为AC或者PE，则后面的测试点跳过|
|SYSTEM_ERROR|9|系统错误，执行log|

系统在得出评测结果时按照以下顺序：
首先查看box是否正常退出，检查是否系统错误。然后检查待测程序返回码和信号量，查看是否RE，然后比较其使用的CPU时间和规定限制，查看是否超时，此后比较其使用的内存和规定限制，查看是否超内存。最后调用matcher或者spj，对比输出和答案，查看是否WA，或者PE。在single result mode下，如果一个测试点为AC或者PE，才会接着评测下一个用例。否则直接退出。



## 报错
Judger产生的所有System Error都将会尽最大努力的存储到指定的log文件中。这里提供错误代码对照。

| 错误名 | 错误代码 | 说明 |
| --- | --- | --- |
|INVALID_JUDGE_CONFIG| -100|评测配置不合法|
|WAIT_BOX_FAILED|-101|无法收集box进程资源统计，|
|WAIT_KILLER_FAILED|-102| 无法wait killer进程终止|
|FORK_BOX_FAILED|-103|无法fork box进程|
|FORK_KILLER_FAILED|-104|无法fork killer进程|
|FORK_KILLER_FOR_SPJ_FAILED|-106|无法fork用于监控spj的killer进程|
|WAIT_SPJ_FAILED|-107|无法收集spj进程资源统计|
|WAIT_KILLER_FOR_SPJ_FAILED|-108|无法wait用于监控spj的killer进程终止|
|COMPILER_RUN_FAILED|-109|编译器无法启动|
|PERMISSION_ERROR|-110|judge进程不是root|
|ACCESS_WORKSPACE_FAILED| -111|无法访问工作区目录|
|BOX_SECURITY_CONFIG_LOAD_FAILED|-200|安全配置加载失败|
|BOX_DATA_REDIRECT_FAILED|-201|评测数据重定向失败|
|BOX_EXE_RUN_FAILED|-202|无法启动待测程序|
|BOX_SET_UID_FATLED|-203|无法设置用户及用户组|
|BOX_SET_LIMIT_FAILED|-204|无法设置资源限制|
|KILLER_SLEEP_FAILED|-300|killer无法阻塞|
|KILLER_KILL_FAILED|-301|killer无法kill掉目标进程|
|MATCHER_OPEN_DATA_FAILED|-400|matcher无法打开数据文件|
|MATCHER_STD_DATA_TOO_LARGE| -401|标准答案文件字符数大于maxCharbuffer|
|LOG_IDENTIFY_ERROR_FAILED| -500|log无法识别错误|
|LOG_FILE_OPEN_FAILED|-501|无法打开log文件|

其中，-50系列报错将直接输出至stderr，其他错误将记录到log文件

## 安全机制
Judger采用seccomp沙盒机制，但是目前还没有配置好，所以在本文档更新之前，请不要将此项目用在公开服务器上。

## SPJ说明
SPJ程序使用C++书写，这里占用SPJ程序的Exit Code作为评测的结果，SPJ的返回值为ACCEPTED，则Judger认为待测程序在此样例下通过，其他情况均为WRONG_ANSWER。SPJ程序需要引入``spjlib.h``文件，然后在开始调用``START_JUDGE()``,另外，``input,output,stdoutput``分别是输入数据，待测程序输出数据以及标准输出的文件指针。在得到评测结果后，应调用``EXIT_JUDGE()``返回结果。无需``return``。这里要注意的是，如果SPJ程序崩溃、超时、超出内存限制，均视为源程序WRONG_ANSWER。这里给一个SPJ程序的示例。
```cpp
#include "spjlib.h"
int main(int argc, char * argv[]) {
    START_JUDGE();


//------------SPJ--CODE--------------------
    int a; fscanf(input, "%d", &a);
    int b; fscanf(output, "%d", &b);
    int c; fscanf(stdoutput, "%d", &c);

    if (/*some conditions*/ )
        EXIT_JUDGE(ACCEPTED); 
    else EXIT_JUDGE(WRONG_ANSWER); 
//------------SPJ--CODE--------------------


    EXIT_JUDGE(ACCEPTED); 
}
```
这里也明确SPJ出现的宏定义常量、函数的意义

```cpp
#define ACCEPTED 0
#define WRONG_ANSWER 1
#define START_JUDGE() FILE * input = fopen(argv[1], "r"); FILE * stdoutput = fopen(argv[2], "r"); FILE * output = fopen(argv[3], "r");
#define EXIT_JUDGE(result) fclose(input); fclose(output); fclose(stdoutput); return result;
```
## 并发评测
本功能在concurent_judge分支下进行，它只是一个实验性的功能，因为我自己也不清楚对于一个题目的样例的并发控制应该由judger负责，还是高层的task queue manager负责。当judgeMode为并发模式下，题目将并发地对一个题目的多个样例进行评测。行为是judger开启多个box和killer进程进行评测。以提高评测效率。最大并发数为MAX_BOX_PROCESS。

## 简单演示

这里对一个java源程序进行评测，源代码如下
 ```java
 import java.util.*;
public class Main {
	public static void main(String[] args) {
		Scanner in = new Scanner(System.in);
		int a = in.nextInt(), b = in.nextInt();
		System.out.println(a+b);
		System.out.println(a-b);
	}
}
 ```
 这里将``Main.java``以及对其评测的数据放到同一目录下，组成工作区。
 
 ![image](https://user-images.githubusercontent.com/44296812/125082918-5c4e1900-e0fa-11eb-8d2d-e3bff7327065.png)

然后配置json文件
```json
{
    "Judger": { 
        "maxCharBuffer" : "10000000"
    },
    "Task" : {
        "taskID":"100031",
        "workSpacePath":"/home/kalzn/Desktop/testworkspace/",
        "logPath":"/home/kalzn/Desktop/testworkspace/log.log",
        "translator": {
            "mode": 2,
            "compilerPath": "/usr/bin/javac",
            "compilerOptions":[
                "javac", "Main.java"
            ],
            "compilerProductName":"Main.class",
            "interpreterPath":"/usr/bin/java",
            "interpreterOptions":[
                "java", "Main"
            ]
        },
        "data" : [
            {
                "inputData":"data/1.in",
                "outputData":"data/output.out",
                "stdAnswer":"data/1.out",             
                "maxCPUTime": 1000,
                "maxMemory" : "102400000000",
                "maxStack" : 1024000
            },
            {
                "inputData":"data/2.in",
                "outputData":"data/output.out",
                "stdAnswer":"data/2.out",           
                "maxCPUTime": 1000,
                "maxMemory" : "102400000000",
                "maxStack" : 1024000
            },
            {
                "inputData":"data/3.in",
                "outputData":"data/output.out",
                "stdAnswer":"data/3.out",               
                "maxCPUTime": 1000,
                "maxMemory" : "102400000000",
                "maxStack" : 1024000
            },
            {
                "inputData":"data/4.in",
                "outputData":"data/output.out",
                "stdAnswer":"data/4.out",            
                "maxCPUTime": 1000,
                "maxMemory" : "102400000000",
                "maxStack" : 1024000
            }
        ]
    }
}
```
评测结束后，工作区目录下生成文件

![image](https://user-images.githubusercontent.com/44296812/125160593-63cff980-e1b0-11eb-8ebf-802aa96244fb.png)

其中``result.json`` 内容如下
```
{
	"taskID":	100031,
	"time":	"2021-07-10 03:35:56",
	"result":	[{
			"time":	65,
			"memory":	37664,
			"signal":	0,
			"code":	0,
			"result":	0,
			"detail":	"No info"
		}, {
			"time":	74,
			"memory":	37524,
			"signal":	0,
			"code":	0,
			"result":	1,
			"detail":	"No info"
		}, {
			"time":	0,
			"memory":	0,
			"signal":	0,
			"code":	0,
			"result":	8,
			"detail":	"No info"
		}, {
			"time":	0,
			"memory":	0,
			"signal":	0,
			"code":	0,
			"result":	8,
			"detail":	"No info"
		}]
}
```
