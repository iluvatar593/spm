#!/bin/bash
rm Farm.zip
zip -r Farm Farm
echo "Uploading"
sshpass -p 'CiaoSole!' scp Farm.zip spm9@131.114.137.225:
echo "Upload complete. Run"

#sshpass -p 'CiaoSole!' ssh spm9@131.114.137.225 "rm -rf ./Farm; unzip Farm.zip; ./run1.sh $1 $2 $3"
