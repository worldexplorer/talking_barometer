@rem for %i in (*.wav) do soxi -D %i >> wav_durations.txt
@rem for %i in (*.wav) do wav_duration %i

@rem echo %1 >> wav_durations.txt
@rem http://stackoverflow.com/questions/7105433/windows-batch-echo-without-new-line
echo|set /p=%1 >> wav_durations.txt
soxi -D %1 >> wav_durations.txt

@rem find: (\d+)\t(\d+) replace:    case $1:\tdelay\($2\); break;