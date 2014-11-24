from django.conf.urls import patterns, url
from wassermanager import views

urlpatterns = patterns('views',
	url(r'^$', 'register_req', name='cooler_register'),
	url(r'^register/$', 'register_req', name='cooler_register'),
	url(r'^login/$', 'login_req', name='cooler_login'),
	url(r'^logout/$', 'logout_req', name='cooler_logout'),
	url(r'^not_logged/$', 'not_logged_in_message', name='cooler_not_logged_in'),
	url(r'^reset/(?P<timestamp>\d+)/$', 'reset', name='cooler_reset'),
	url(r'^reset/(?P<timestamp>\d+)/(?P<bottle_size>\d+)/$', 'reset', name='cooler_reset'),
	url(r'^spend/$', 'spend', name='spent'),
	url(r'^spend/(?P<timestamp>\d+)/(?P<duration>[\d\.])/(?P<amount>\d{3}.\d{2})/$', 'batchspend'),
)

#other groups a made using next synttax:
# urlpatterns += patterns(...)
