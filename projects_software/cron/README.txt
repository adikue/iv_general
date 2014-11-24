Crontab parser <cron_next_run.py>
Parse crontab and print next closest runs for all tasks. 

Directory contains <crontab> file. It is crontab for testing purposes. Fell free to use it.
For more information look into sources or contact with Ivan Vovk (adikue@gmail.com)

Limitations:
	Now this script doesn't support '/' symbol in crontab files.
	Not supported 'day of week' property in crontab files.
	Please be aware of these.

Requirements:
	python 2.7.5

Usage: 
	#python cron_next_run.py -i <crontab>