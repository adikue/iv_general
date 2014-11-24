# -*- coding: utf-8 -*-
from south.utils import datetime_utils as datetime
from south.db import db
from south.v2 import SchemaMigration
from django.db import models


class Migration(SchemaMigration):

    def forwards(self, orm):
        # Deleting field 'Spending.cooller'
        db.delete_column('wassermanager_spending', 'cooller_id')

        # Adding field 'Spending.cooler'
        db.add_column('wassermanager_spending', 'cooler',
                      self.gf('django.db.models.fields.related.ForeignKey')(to=orm['wassermanager.Cooler'], default=0),
                      keep_default=False)


    def backwards(self, orm):

        # User chose to not deal with backwards NULL issues for 'Spending.cooller'
        raise RuntimeError("Cannot reverse this migration. 'Spending.cooller' and its values cannot be restored.")
        
        # The following code is provided here to aid in writing a correct migration        # Adding field 'Spending.cooller'
        db.add_column('wassermanager_spending', 'cooller',
                      self.gf('django.db.models.fields.related.ForeignKey')(to=orm['wassermanager.Cooler']),
                      keep_default=False)

        # Deleting field 'Spending.cooler'
        db.delete_column('wassermanager_spending', 'cooler_id')


    models = {
        'auth.group': {
            'Meta': {'object_name': 'Group'},
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'unique': 'True', 'max_length': '80'}),
            'permissions': ('django.db.models.fields.related.ManyToManyField', [], {'blank': 'True', 'to': "orm['auth.Permission']", 'symmetrical': 'False'})
        },
        'auth.permission': {
            'Meta': {'object_name': 'Permission', 'unique_together': "(('content_type', 'codename'),)", 'ordering': "('content_type__app_label', 'content_type__model', 'codename')"},
            'codename': ('django.db.models.fields.CharField', [], {'max_length': '100'}),
            'content_type': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['contenttypes.ContentType']"}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '50'})
        },
        'auth.user': {
            'Meta': {'object_name': 'User'},
            'date_joined': ('django.db.models.fields.DateTimeField', [], {'default': 'datetime.datetime.now'}),
            'email': ('django.db.models.fields.EmailField', [], {'blank': 'True', 'max_length': '75'}),
            'first_name': ('django.db.models.fields.CharField', [], {'blank': 'True', 'max_length': '30'}),
            'groups': ('django.db.models.fields.related.ManyToManyField', [], {'blank': 'True', 'to': "orm['auth.Group']", 'related_name': "'user_set'", 'symmetrical': 'False'}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'is_active': ('django.db.models.fields.BooleanField', [], {'default': 'True'}),
            'is_staff': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'is_superuser': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'last_login': ('django.db.models.fields.DateTimeField', [], {'default': 'datetime.datetime.now'}),
            'last_name': ('django.db.models.fields.CharField', [], {'blank': 'True', 'max_length': '30'}),
            'password': ('django.db.models.fields.CharField', [], {'max_length': '128'}),
            'user_permissions': ('django.db.models.fields.related.ManyToManyField', [], {'blank': 'True', 'to': "orm['auth.Permission']", 'related_name': "'user_set'", 'symmetrical': 'False'}),
            'username': ('django.db.models.fields.CharField', [], {'unique': 'True', 'max_length': '30'})
        },
        'contenttypes.contenttype': {
            'Meta': {'db_table': "'django_content_type'", 'object_name': 'ContentType', 'unique_together': "(('app_label', 'model'),)", 'ordering': "('name',)"},
            'app_label': ('django.db.models.fields.CharField', [], {'max_length': '100'}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'model': ('django.db.models.fields.CharField', [], {'max_length': '100'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '100'})
        },
        'wassermanager.cooler': {
            'Meta': {'object_name': 'Cooler'},
            'customer': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['wassermanager.Customer']"}),
            'default_bottle_size': ('django.db.models.fields.PositiveSmallIntegerField', [], {}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'reset_time': ('django.db.models.fields.DateTimeField', [], {'blank': 'True', 'auto_now_add': 'True'}),
            'user': ('django.db.models.fields.related.OneToOneField', [], {'unique': 'True', 'to': "orm['auth.User']"}),
            'watter_amount': ('django.db.models.fields.DecimalField', [], {'decimal_places': '3', 'max_digits': '5', 'default': "'0'"})
        },
        'wassermanager.customer': {
            'Meta': {'object_name': 'Customer'},
            'bottles_available': ('django.db.models.fields.PositiveSmallIntegerField', [], {}),
            'distributor': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['wassermanager.Distributor']"}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'user': ('django.db.models.fields.related.OneToOneField', [], {'unique': 'True', 'to': "orm['auth.User']"})
        },
        'wassermanager.distributor': {
            'Meta': {'object_name': 'Distributor'},
            'bottles_available': ('django.db.models.fields.PositiveIntegerField', [], {}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'user': ('django.db.models.fields.related.OneToOneField', [], {'unique': 'True', 'to': "orm['auth.User']"})
        },
        'wassermanager.order': {
            'Meta': {'object_name': 'Order'},
            'bottles_amount': ('django.db.models.fields.PositiveSmallIntegerField', [], {}),
            'customer': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['wassermanager.Customer']"}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'})
        },
        'wassermanager.spending': {
            'Meta': {'object_name': 'Spending'},
            'amount': ('django.db.models.fields.DecimalField', [], {'decimal_places': '3', 'max_digits': '5'}),
            'cooler': ('django.db.models.fields.related.ForeignKey', [], {'to': "orm['wassermanager.Cooler']"}),
            'duration': ('django.db.models.fields.FloatField', [], {}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'start_time': ('django.db.models.fields.DateTimeField', [], {})
        }
    }

    complete_apps = ['wassermanager']