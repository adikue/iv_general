from django.contrib import admin
from django.contrib.auth.models import User

from wassermanager.models import Distributor, Customer, Cooler

admin.site.register(Distributor)
admin.site.register(Customer)
admin.site.register(Cooler)
