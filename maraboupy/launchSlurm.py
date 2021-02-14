
import os
import sys
import subprocess
from datetime import datetime
import json

def experimentCNNAbsVsVanilla(numRunsPerType, commonFlags, batchDirPath):

    TIMEOUT_H, TIMEOUT_M, TIMEOUT_S = 12, 0, 0
    
    runCmds = list()
    runTitles = list()
    runBriefs = list()    

    for i in range(numRunsPerType):
        title = "MaskCOICfg---{}".format(i)
        runCmds.append(commonFlags + ["--run_title", title, "--sample", str(i)])
        runTitles.append(title)
        runBriefs.append("Run with CNN improvments")
        
    for i in range(numRunsPerType):
        title = "VanillaCfg---{}".format(i)
        runCmds.append(commonFlags + ["--run_title", title, "--sample", str(i), "--no_coi", "--no_mask"])
        runTitles.append(title)
        runBriefs.append('Run with default ("vanilla") Marabou')
        
    with open(batchDirPath + "/plotSpec.json", 'w') as f:
        jsonDict = {"TIMEOUT_VAL" : TIMEOUT_H * 3600 + TIMEOUT_M * 60 + TIMEOUT_S,
                    "title2Label" : {'MaskCOICfg' : 'CNN Abstraction', 'VanillaCfg' : 'Vanilla Marabou'},
                    "COIRatio"    : ['MaskCOICfg'],
                    "compareProperties": [('VanillaCfg', 'MaskCOICfg')]}
        json.dump(jsonDict, f, indent = 4)

    TIME_LIMIT = "12:00:00".format(TIMEOUT_H, TIMEOUT_M, TIMEOUT_S)

    return runCmds, runTitles, runBriefs, TIME_LIMIT

####################################################################################################
####################################################################################################
####################################################################################################

batchId = "slurm_" + datetime.now().strftime("%d-%m-%y___%H-%M-%S")
basePath = "/cs/labs/guykatz/matanos/Marabou/maraboupy/"
if not os.path.exists(basePath + "logs/"):
    os.mkdir(basePath + "logs/")
batchDirPath = basePath + "logs/" + batchId
if not os.path.exists(batchDirPath):
    os.mkdir(batchDirPath)
    
CPUS = 8
MEM_PER_CPU = "1G"
commonFlags = ["--run_on", "cluster", "--batch_id", batchId, "--sporious_strict", "--num_cpu", str(CPUS)]
numRunsPerType = 50

runCmds, runTitles, runBriefs, TIME_LIMIT = experimentCNNAbsVsVanilla(numRunsPerType, commonFlags, batchDirPath)
sbatchFiles = list()
for cmd, title, brief in zip(runCmds, runTitles, runBriefs):

    runDirPath = batchDirPath + "/" + title
    if not os.path.exists(runDirPath):
        os.mkdir(runDirPath)
    os.chdir(runDirPath)

    sbatchCode = list()
    sbatchCode.append("#!/bin/bash")
    sbatchCode.append("#SBATCH --job-name=cnnAbsTB_{}_{}".format(batchId, title))
    sbatchCode.append("#SBATCH --cpus-per-task={}".format(CPUS))
    sbatchCode.append("#SBATCH --mem-per-cpu={}".format(MEM_PER_CPU))
    sbatchCode.append("#SBATCH --output={}/cnnAbsTB_{}.out".format(runDirPath, title))
    sbatchCode.append("#SBATCH --partition=long")
    sbatchCode.append("#SBATCH --signal=B:SIGUSR1@300")
    sbatchCode.append("#SBATCH --time={}".format(TIME_LIMIT))
    sbatchCode.append("#SBATCH -C avx2")
    #sbatchCode.append("#SBATCH --reservation 5781")    
    sbatchCode.append("")
    sbatchCode.append("pwd; hostname; date")
    sbatchCode.append("")
    sbatchCode.append("csh /cs/labs/guykatz/matanos/py_env/bin/activate.csh")
    sbatchCode.append("export PYTHONPATH=$PYTHONPATH:/cs/labs/guykatz/matanos/Marabou")
    sbatchCode.append("export GUROBI_HOME=/cs/labs/guykatz/matanos/gurobi900/linux64")
    sbatchCode.append("export PATH=$PATH:${GUROBI_HOME}/bin")
    sbatchCode.append("export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib")
    sbatchCode.append("export GRB_LICENSE_FILE=/cs/share/etc/license/gurobi/gurobi.lic")
    sbatchCode.append("")
    sbatchCode.append("### Description of this specific run is : {}".format(brief))
    sbatchCode.append("")
    sbatchCode.append('echo "Ive been launched" > {}/Started'.format(runDirPath))        
    sbatchCode.append("stdbuf -o0 -e0 python3 /cs/labs/guykatz/matanos/Marabou/maraboupy/cnn_abs_testbench.py {}".format(" ".join(cmd)))
    sbatchCode.append("")
    sbatchCode.append("date")

    sbatchFiles.append(runDirPath + "/" + "cnnAbsRun-{}.sbatch".format(title))
    with open(sbatchFiles[-1], "w") as f:
        for line in sbatchCode:
            f.write(line + "\n")

os.chdir(basePath)

for f in sbatchFiles:
    print("Running : {}".format(" ".join(["sbatch", f])))
    subprocess.run(["sbatch", f])
