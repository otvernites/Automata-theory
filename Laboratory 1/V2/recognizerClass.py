import re


def is_int(str):
    try:
        int(str)
        return True
    except ValueError:
        return False


def string_analysis(s):
    return re.findall(r'\bnfs://[a-zA-Z]{1,15}/(?:([a-zA-Z._]{1,20}/([a-zA-Z._]{1,12})|([a-zA-Z._]{1,12})))\b', s)


count = input('Enter the number of lines to check: ')
while not is_int(count):
    print('Try again')
    count = input('Enter the number of lines to check: ')

str_mas = []
statist = {}
for i in range(int(count)):
    print('Enter the string:')
    str_mas.append(input())

for i in str_mas:
    s = string_analysis(i)
    if not s:
        print(i + ' - is unacceptable')
    elif i[-1] == '/' and s:
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