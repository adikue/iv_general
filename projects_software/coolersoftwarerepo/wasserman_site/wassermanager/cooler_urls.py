from django.conf.urls import patterns, url

urlpatterns = patterns('wassermanager.cooler_views',
	url(r'^$', 'register_req', name='register'),
	url(r'^register/$', 'register_req', name='register'),
	url(r'^login/$', 'login_req', name='login'),
	url(r'^logout/$', 'logout_req', name='logout'),
	url(r'^not_logged/.*', 'not_logged_in_message', name='not_logged'),
	url(r'^profile/$', 'profile', name='profile'),
	url(r'^reset/$', 'reset', name='reset'),
	url(r'^reset/(?P<timestamp>\d+)/$', 'reset', name='reset'),
	url(r'^reset/(?P<timestamp>\d+)/(?P<bottle_size>\d+)/$', 'reset', name='reset'),
	url(r'^spend/$', 'batchspend'),
	url(r'^spend/(?P<timestamp>\d+)/(?P<duration>[\d.]+)/(?P<amount>\d{2}.\d{3})/$', 'spend', name='spend'),
	url(r'^spend/(?P<duration>[\d.]+)/(?P<amount>\d{2}.\d{3})/$', 'spend', name='spend'),
)

#other groups a made using next synttax:
# urlpatterns += patterns(...)
