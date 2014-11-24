from decimal import *

from django.db import models
from django.contrib.auth.models import User

class Distributor(models.Model):
	user = models.OneToOneField(User)
	bottles_available = models.PositiveIntegerField()
	def __str__(self):
		return self.user.username
	
class Customer(models.Model):
	user = models.OneToOneField(User)
	bottles_available = models.PositiveSmallIntegerField()
	distributor = models.ForeignKey(Distributor)
	def __str__(self):
		return self.user.username

class Order(models.Model):
	bottles_amount = models.PositiveSmallIntegerField()
	customer = models.ForeignKey(Customer)

class Cooler(models.Model):
	user = models.OneToOneField(User)
	watter_amount = models.DecimalField(max_digits=5, decimal_places=3, default=Decimal(0))
	reset_time = models.DateTimeField(auto_now_add=True, help_text='last bottle change time')
	default_bottle_size = models.PositiveSmallIntegerField()
	customer = models.ForeignKey(Customer)

	def __str__(self):
		return self.user.username
	# TODO add geoposition

class Spending(models.Model):
	amount = models.DecimalField(max_digits=5, decimal_places=3)
	start_time = models.DateTimeField('water spending start time')
	duration = models.FloatField() 
	cooler = models.ForeignKey(Cooler)
