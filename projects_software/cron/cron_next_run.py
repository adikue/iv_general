from collections import namedtuple
from datetime import datetime, timedelta
import re, sys, getopt

###################################################### PRE VALUES ############################################################

#Output format for date and time
dt_format = "%d.%m.%Y %H:%M:%S"

#timedelta object properties
full_date_attrs = ['year', 'month', 'day', 'hour', 'minute']
time_delta_available = ['days', 'seconds', 'microseconds', 'milliseconds', 'minutes', 'hours', 'weeks']

#namedtuple for carring crontab records
task_cron_attrs = 'year, month, day, hour, minute'
task_cron_attrs_list = task_cron_attrs.split(', ')
task_cron_rec = namedtuple("task_cron_rec", task_cron_attrs)

#Pattern for matching comment lines
comment_line = re.compile(r'^#')

###################################################### FUNCTIONS ############################################################

def print_next_runs(next_runs):
	"""Helper function. Just print computed next runs in propper format """
	r_string = "Commands next runs:\n"
	for cmd in next_runs.keys():
		r_string = r_string + next_runs[cmd].strftime(dt_format) + " | " + cmd + "\n"
	print r_string

def gen_timedelta_attrs(inc_val):
	"""Return dictionary with values for 'timedelta' attributes with only 1 in 'inc_val'attribute """
	attrs = dict()
	for method_key in time_delta_available:
		if method_key == inc_val:
			attrs[method_key] = 1
		else:	
			attrs[method_key] = 0
	return attrs

def get_asterix_list(_task_cron_record):
	"""Return list with names of asterixed attrubites in task_cron_rec"""
	return [k for k, v in _task_cron_record._asdict().iteritems() if v == '*']

def get_now_attrs(asterix_attrs_names):
	"""Return dictionary with today values for attributes in 'asterix_attrs_names'"""
	attrs = dict()
	for method_key in asterix_attrs_names:
		attrs[method_key] = getattr(datetime.now(), method_key)
	return attrs

def convert(string_val):
	"""Fucntions converts crontab values into list if integers or '*' """
	r_list = list()

	if '/' in string_val:
		raise ValueError("'/' symbol is NOT supported yet")

	if string_val == '*':
		r_list.append('*')
	elif ',' in string_val:#list
		for val in string_val.split(','):
			r_list.append(int(val))
	elif '-' in string_val:#range
		rang = string_val.split('-')
		if len(rang) > 2:
			raise ValueError("Incorrect crontab format")
		min_v = int(rang[0])
		max_v = int(rang[1])
		r_list = range(min_v, max_v+1)
	else:
		try:
			int(string_val)
		except ValueError:
			raise ValueError("Incorrect crontab format")
		r_list.append(int(string_val))
	return r_list

def gen_today_schedule(_cron_task_nt):
	"""Fill task_cron_rec attributes with '*'' today values"""
	#get list with names of attributes with '*'
	asterix_list = get_asterix_list(_cron_task_nt)
	#get today values for asterix attributes
	today_attrs = get_now_attrs(asterix_list)

	#List with arguents for creation task_cron_rec
	nowadate = list()
	#Fill task_cron_rec asterix attributes with today values and other with proper values
	for attr in full_date_attrs:#cycle in full set of datettime attributes
		if attr in asterix_list:#if attribute fot this task is "*"
			nowadate.append(today_attrs[attr])#Set it for nowadate attr
		else:
			nowadate.append(getattr(_cron_task_nt,attr))#Else set it like in task record

	return task_cron_rec(*nowadate)

def find_future_runs(research_tasks_list):
	"""Fucntion searching for next nearest run for the tasks in 'research_tasks_list'"""
	tasks_nt = [a for a,b,c in research_tasks_list]
	ast_lists = [b for a,b,c in research_tasks_list]
	commands_list = [c for a,b,c in research_tasks_list]

	future_runs = dict()

	for n,task_nt in enumerate(tasks_nt):
		__ast_list = [x+'s' for x in ast_lists[n]]
		if len(__ast_list) >= 1: # if there is possibility for next run
			correct_date = False
			if __ast_list[-1] in time_delta_available: #if for shortest variable attribute timedelta available
				# generate timedelta object for increment shortest variable attribute
				increment = timedelta(**gen_timedelta_attrs(__ast_list[-1]))
				next_run = datetime(*task_nt) + increment
				correct_date = True
				#append next run to return list
			else:
				var_attr = getattr(task_nt, ast_lists[n][-1])# take shortest variable argument
				if ast_lists[n][-1] == 'month' and var_attr < 12:#if variable is month and it can be incremented
					#get values list from task_nt
					attrs_list = list()
					for attr in task_cron_attrs_list: 
						x=getattr(task_nt, attr)
						attrs_list.append(x)
					correct_date = False	
					while not correct_date:
						correct_date = True
						#increment 
						var_attr = var_attr + 1
						if var_attr > 12:# if not avail in this year
							correct_date = False
							break
						#set propper attribute to var_attr (increment it)
						attrs_list[task_cron_attrs_list.index(ast_lists[n][-1])] = var_attr
						try:
							next_run = datetime(*task_cron_rec(*attrs_list))
						except ValueError:
							correct_date = False
			if correct_date:
				if future_runs.has_key(' '.join(commands_list[n])):
					future_runs[' '.join(commands_list[n])].append(next_run)
				else:
					future_runs[' '.join(commands_list[n])] = [next_run]

	return future_runs

###################################################### SCRIPT ############################################################

#List with tasks wich need research for future run possibility
for_research = list()
#Dictonaries for today and not today tasks
today_tasks = dict()
future_tasks = dict()

#Parsing console arguments and opening crontab file
crontab_file = 'crontab'
try:
	opts, args = getopt.getopt(sys.argv[1:],"h:i:")
except getopt.GetoptError as e:
	print (str(e))
   	print 'Incorrect crontab file. Usage: cron_next_run.py -i <crontab>'
   	sys.exit(2)

for opt, arg in opts:
	if opt == '-h':
   		print 'Usage: cron_next_run.py -i <crontab>'
   		sys.exit()
   	elif opt == "-i":
   		crontab_file = arg

cron_tab_lines = open(crontab_file, 'r').readlines()

for line in cron_tab_lines:
	#print line,

	if not comment_line.match(line):
		tokens = line.split();
		if len(tokens) < 6:
			raise ValueError("Incorrect crontab format")

		#Parse line in crontab
		minute = convert(tokens[0])
		hour = convert(tokens[1])
		dom = convert(tokens[2])
		month = convert(tokens[3])
		command = tokens[5:]
		#Get current year
		year = datetime.now().year

		#Generate task according to crontab schedule
		for mo in month:
			for d in dom:
				for h in hour:
					for mi in minute:
						__task_cron_rec = task_cron_rec(year, mo, d, h, mi)
						#Set available variables to today's one
						today_taks_rec = gen_today_schedule(__task_cron_rec)
						task_is_correct_for_today = False
						#Check if task record is correct for today
						try:
							task_is_correct_for_today = True
							task_today_schedule = datetime(*today_taks_rec)
						except ValueError:
							task_is_correct_for_today = False
				
						#If current task is correct and will happen today
						if task_is_correct_for_today and task_today_schedule > datetime.now():
							#Add it into next_run_list
							if today_tasks.has_key(' '.join(command)):
								today_tasks[' '.join(command)].append(task_today_schedule)
							else:
								today_tasks[' '.join(command)] = [task_today_schedule]
						else:
							#Add task in list for research for future hapenning
							asterix_list = get_asterix_list(__task_cron_rec)
							for_research.append((today_taks_rec, asterix_list, command))

#Do future happening research and add future tasks in next_run_list
future_tasks = find_future_runs(for_research)

#Catenate toady and future tasks 
full_next_runs = dict()
for cmd in today_tasks.keys():
	full_next_runs[cmd] = min(today_tasks[cmd])
for cmd in future_tasks.keys():
	full_next_runs[cmd] = min(future_tasks[cmd])

#Print all nearest future runs of tasks
print_next_runs(full_next_runs)