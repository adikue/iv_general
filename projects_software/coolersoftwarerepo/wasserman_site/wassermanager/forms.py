from django import forms
from django.forms import ModelForm
from django.contrib.auth.models import User

from wassermanager.models import Cooler

class CoolerRegistrationForm(ModelForm):
	class Meta:
		model = Cooler
		exclude=('user', 'watter_amount',)
