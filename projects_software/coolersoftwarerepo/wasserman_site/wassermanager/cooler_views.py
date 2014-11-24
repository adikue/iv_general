import re
import datetime
from decimal import *

from django.shortcuts import render, get_object_or_404, render_to_response
from django.views.generic import View
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.decorators import login_required
from django.contrib.auth.forms import UserCreationForm, AuthenticationForm
from django.http import Http404, HttpResponse, HttpResponseNotFound, HttpResponseBadRequest, HttpResponseForbidden, HttpResponseRedirect
from django.core.urlresolvers import reverse, reverse_lazy
from django.template import RequestContext

from wassermanager.models import Distributor, Customer, Order, Cooler, Spending
from wassermanager.forms import CoolerRegistrationForm

def __ok():
	return HttpResponse('OK')

def __make_datetime(timestamp):
	return datetime.datetime.fromtimestamp(float(timestamp)) if timestamp is not None else datetime.datetime.now()

# FIXME That is a bullshit!!! No registration for coolers should be. Only distributor may register cooler!!!
# consider activation from customer/distributor side?
def register_req(request):
	if request.user.is_authenticated():
		return HttpResponse('is authenticated') # TODO consider return some another value
	if request.method == 'POST':
		uform = UserCreationForm(request.POST)
		cform = CoolerRegistrationForm(request.POST)
		if cform.is_valid() and uform.is_valid():
			user = uform.save()
			cooler = cform.save(commit=False)
			cooler.user = user
			cooler.save()
			return __ok() 
		else:
			'''resubmit the form'''
			return render_to_response(
				'wassermanager/cooler_register.html',
				{'cooler_form': cform, 'user_form': uform},
				context_instance=RequestContext(request)
			)
	else:
		'''User is not submiting the form'''
		cform = CoolerRegistrationForm()
		uform = UserCreationForm()
		return render_to_response('wassermanager/cooler_register.html', {'user_form': uform, 'cooler_form': cform}, context_instance=RequestContext(request))
		
	

def login_req(request):
	if request.user.is_authenticated():
		return HttpResponse('is authenticated') # TODO consider return some another value
	# TODO add is active here. Activation is provided by distributor or customer
	if request.method == 'POST':
		form = AuthenticationForm(request, request.POST)
		if form.is_valid():
			login(request, form.get_user())
			return __ok() # signal 200 that everything is ok
		else:
			return render_to_response('wassermanager/cooler_auth.html', {'form': form}, context_instance=RequestContext(request))
			#return HttpResponseBadRequest('POST required') # signal 400 that error form or post request required 
	else:
		return render_to_response('wassermanager/cooler_auth.html', {'form': AuthenticationForm}, context_instance=RequestContext(request))
		#return HttpResponseBadRequest('POST required') # signal 400 that error form or post request required

def logout_req(request):
	logout(request)
	return HttpResponse('Logged out')

def not_logged_in_message(request):
	return HttpResponseForbidden('Need authorisation') # signal 403 that login is required

@login_required(login_url=reverse_lazy('cooler:not_logged'))
def reset(request, timestamp=None, bottle_size=None):
	c = request.user.cooler
	c.reset_time = __make_datetime(timestamp) 
	c.default_bottle_size = Decimal(bottle_size) if bottle_size is not None else c.default_bottle_size
	c.watter_amount = c.default_bottle_size
	c.save()
	return __ok()

@login_required(login_url=reverse_lazy('cooler:not_logged'))
def spend(request, duration, amount, timestamp=None, ):
	c = request.user.cooler
	dt = __make_datetime(timestamp) 
	sp = Spending(amount=amount, start_time=dt, duration=duration, cooler=c)
	sp.save()
	return __ok()

@login_required(login_url=reverse_lazy('cooler:not_logged'))
def batchspend(request):
	i = 0
	for line in request:
		++i
		# Aha!! code duplication in pattern matching
		m = re.search(r'(?P<timestamp>\d+)\s(?P<duration>[\d\.])\s(?P<amount>\d{3}.\d{2})', line)
		if m:
			spend(request,
				timestamp=m.group('timestamp'),
				duration=m.group('duration'),
				amount=m.group('amount')
			)
		else:
			return HttpResponseBadRequest(
				'Check format of submitted file, error found at line:{}: {} '.format(i, line)
			)
	return __ok()

from chartit import DataPool, Chart

@login_required(login_url=reverse_lazy('cooler:not_logged'))
def profile(request):
	spendings = Spending.objects.filter(cooler = request.user.cooler)
	ds = DataPool(series=[{
			'options': {'source': Spending.objects.filter(cooler = request.user.cooler)},
			'terms': ['start_time', 'amount']
		}]
	)
	cht = Chart(datasource = ds,
		series_options = [{
			'options': {
				'type': 'line',
				'stacking': False},
				'terms': {
					'start_time': [
						'amount',
					]
				}
		}],
		chart_options = {
			'title': {
				'text': 'Watter spendings'
			},
			'xAxis': {
				'title': {
					'text': 'time'
				}
			},
			'yAxis': {
				'title': {
					'text': 'water spent'
				}
			}
		}
	)
	return render_to_response('wassermanager/cooler_profile.html', {'spendings_chart': cht})
