# -*- coding: utf-8 -*-
from south.utils import datetime_utils as datetime
from south.db import db
from south.v2 import SchemaMigration
from django.db import models


class Migration(SchemaMigration):

    def forwards(self, orm):
        # Adding model 'Distributor'
        db.create_table('wassermanager_distributor', (
            ('id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('user', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['auth.User'])),
            ('bottles_available', self.gf('django.db.models.fields.PositiveIntegerField')()),
        ))
        db.send_create_signal('wassermanager', ['Distributor'])

        # Adding model 'Customer'
        db.create_table('wassermanager_customer', (
            ('id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('user', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['auth.User'])),
            ('bottles_available', self.gf('django.db.models.fields.PositiveSmallIntegerField')()),
            ('distributor', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['wassermanager.Distributor'])),
        ))
        db.send_create_signal('wassermanager', ['Customer'])

        # Adding model 'Order'
        db.create_table('wassermanager_order', (
            ('id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('bottles_amount', self.gf('django.db.models.fields.PositiveSmallIntegerField')()),
            ('customer', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['wassermanager.Customer'])),
        ))
        db.send_create_signal('wassermanager', ['Order'])

        # Adding model 'Cooler'
        db.create_table('wassermanager_cooler', (
            ('id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('user', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['auth.User'])),
            ('watter_amount', self.gf('django.db.models.fields.DecimalField')(max_digits=5, decimal_places=3)),
            ('setup_time', self.gf('django.db.models.fields.DateTimeField')()),
            ('customer', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['wassermanager.Customer'])),
        ))
        db.send_create_signal('wassermanager', ['Cooler'])

        # Adding model 'Spending'
        db.create_table('wassermanager_spending', (
            ('id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('amount', self.gf('django.db.models.fields.DecimalField')(max_digits=5, decimal_places=3)),
            ('start_time', self.gf('django.db.models.fields.DateTimeField')()),
            ('duration', self.gf('django.db.models.fields.FloatField')()),
            ('cooller', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['wassermanager.Cooler'])),
        ))
        db.send_create_signal('wassermanager', ['Spending'])


    def backwards(self, orm):
        # Deleting model 'Distributor'
        db.delete_table('wassermanager_distributor')

        # Deleting model 'Customer'
        db.delete_table('wassermanager_customer')

        # Deleting model 'Order'
        db.delete_table('wassermanager_order')

        # Deleting model 'Cooler'
        db.delete_table('wassermanager_cooler')

        # Deleting model 'Spending'
        db.delete_table('wassermanager_spending')


    models = {
        'auth.group': {
            'Meta': {'object_name': 'Group'},
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'unique': 'True', 'max_length': '80'}),
            'permissions': ('django.db.models.fields.related.ManyToManyField', [], {'symmetrical': 'False', 'to': "orm['auth.Permission']", 'blank': 'True'})
        },
        'auth.permission': {
            'Meta': {'unique_together': "(('content_type', 'codename'),)", 'ordering': "('content_type__app_label', 'content_type__model', 'codename')", 'object_name': 'Permission'},
            'codename': ('django.db.models.fields.CharField', [], {'max_length': '100'}),
            'content_type': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['contenttypes.ContentType']"}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '50'})
        },
        'auth.user': {
            'Meta': {'object_name': 'User'},
            'date_joined': ('django.db.models.fields.DateTimeField', [], {'default': 'datetime.datetime.now'}),
            'email': ('django.db.models.fields.EmailField', [], {'max_length': '75', 'blank': 'True'}),
            'first_name': ('django.db.models.fields.CharField', [], {'max_length': '30', 'blank': 'True'}),
            'groups': ('django.db.models.fields.related.ManyToManyField', [], {'symmetrical': 'False', 'related_name': "'user_set'", 'to': "orm['auth.Group']", 'blank': 'True'}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'is_active': ('django.db.models.fields.BooleanField', [], {'default': 'True'}),
            'is_staff': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'is_superuser': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'last_login': ('django.db.models.fields.DateTimeField', [], {'default': 'datetime.datetime.now'}),
            'last_name': ('django.db.models.fields.CharField', [], {'max_length': '30', 'blank': 'True'}),
            'password': ('django.db.models.fields.CharField', [], {'max_length': '128'}),
            'user_permissions': ('django.db.models.fields.related.ManyToManyField', [], {'symmetrical': 'False', 'related_name': "'user_set'", 'to': "orm['auth.Permission']", 'blank': 'True'}),
            'username': ('django.db.models.fields.CharField', [], {'unique': 'True', 'max_length': '30'})
        },
        'contenttypes.contenttype': {
            'Meta': {'unique_together': "(('app_label', 'model'),)", 'ordering': "('name',)", 'object_name': 'ContentType', 'db_table': "'django_content_type'"},
            'app_label': ('django.db.models.fields.CharField', [], {'max_length': '100'}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'model': ('django.db.models.fields.CharField', [], {'max_length': '100'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '100'})
        },
        'wassermanager.cooler': {
            'Meta': {'object_name': 'Cooler'},
            'customer': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['wassermanager.Customer']"}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'setup_time': ('django.db.models.fields.DateTimeField', [], {}),
            'user': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['auth.User']"}),
            'watter_amount': ('django.db.models.fields.DecimalField', [], {'max_digits': '5', 'decimal_places': '3'})
        },
        'wassermanager.customer': {
            'Meta': {'object_name': 'Customer'},
            'bottles_available': ('django.db.models.fields.PositiveSmallIntegerField', [], {}),
            'distributor': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['wassermanager.Distributor']"}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'user': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['auth.User']"})
        },
        'wassermanager.distributor': {
            'Meta': {'object_name': 'Distributor'},
            'bottles_available': ('django.db.models.fields.PositiveIntegerField', [], {}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'user': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['auth.User']"})
        },
        'wassermanager.order': {
            'Meta': {'object_name': 'Order'},
            'bottles_amount': ('django.db.models.fields.PositiveSmallIntegerField', [], {}),
            'customer': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['wassermanager.Customer']"}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'})
        },
        'wassermanager.spending': {
            'Meta': {'object_name': 'Spending'},
            'amount': ('django.db.models.fields.DecimalField', [], {'max_digits': '5', 'decimal_places': '3'}),
            'cooller': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['wassermanager.Cooler']"}),
            'duration': ('django.db.models.fields.FloatField', [], {}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'start_time': ('django.db.models.fields.DateTimeField', [], {})
        }
    }

    complete_apps = ['wassermanager']