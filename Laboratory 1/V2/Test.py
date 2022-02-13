import re
import time

def string_analysis(s):
    return re.findall(r'\bnfs://[a-zA-Z]{1,15}/(?:([a-zA-Z._]{1,20}/([a-zA-Z._]{1,12})|([a-zA-Z._]{1,12})))\b', s)


str_mas = open('25000.txt', 'r')
statist = {}
all_time = []

for i in str_mas:
    start_time = time.process_time_ns()
    s = string_analysis(i)
    end_time = time.process_time_ns()
    all_time.append((end_time-start_time))

    if not s:
        print(i + ' - is unacceptable')
    elif i[-2] == '/' and s:
        print(i + ' - is unacceptable')
    else:
        print(i + ' -  is acceptable')
        if s[0][1] == '':
            f_name = s[0][2]
            if f_name in statist.keys():
                statist[f_name] += 1
            else:
                statist[f_name] = 1
        else:
            f_name = s[0][1]
            if f_name in statist.keys():
                statist[f_name] += 1
            else:
                statist[f_name] = 1

print(statist)
print(sum(all_time))


