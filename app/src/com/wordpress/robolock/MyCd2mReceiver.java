package com.wordpress.robolock;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences.Editor;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

public class MyCd2mReceiver extends BroadcastReceiver{
	private static final String KEY = "c2dmPref";
	private static final String REGISTRATION_KEY = "registrationKey";

	private Context context;
	
	@Override
	public void onReceive(Context context, Intent intent) {
	    this.context = context;
		if (intent.getAction().equals("com.google.android.c2dm.intent.REGISTRATION")) {
	        handleRegistration(context, intent);
	    } else if (intent.getAction().equals("com.google.android.c2dm.intent.RECEIVE")) {
	        handleMessage(context, intent);
	    }
	 }

	private void handleRegistration(Context context, Intent intent) {
	    String registration = intent.getStringExtra("registration_id");
	    if (intent.getStringExtra("error") != null) {
	        // Registration failed, should try again later.
		    Log.d("c2dm", "registration failed");
		    String error = intent.getStringExtra("error");
		    if(error == "SERVICE_NOT_AVAILABLE"){
		    	Log.d("c2dm", "SERVICE_NOT_AVAILABLE");
		    }else if(error == "ACCOUNT_MISSING"){
		    	Log.d("c2dm", "ACCOUNT_MISSING");
		    }else if(error == "AUTHENTICATION_FAILED"){
		    	Log.d("c2dm", "AUTHENTICATION_FAILED");
		    }else if(error == "TOO_MANY_REGISTRATIONS"){
		    	Log.d("c2dm", "TOO_MANY_REGISTRATIONS");
		    }else if(error == "INVALID_SENDER"){
		    	Log.d("c2dm", "INVALID_SENDER");
		    }else if(error == "PHONE_REGISTRATION_ERROR"){
		    	Log.d("c2dm", "PHONE_REGISTRATION_ERROR");
		    }
	    } else if (intent.getStringExtra("unregistered") != null) {
	        // unregistration done, new messages from the authorized sender will be rejected
	    	Log.d("c2dm", "unregistered");

	    } else if (registration != null) {
	    	Log.d("c2dm", registration);
	    	Editor editor =
                context.getSharedPreferences(KEY, Context.MODE_PRIVATE).edit();
            editor.putString(REGISTRATION_KEY, registration);
    		editor.commit();
	       // Send the registration ID to the 3rd party site that is sending the messages.
	       // This should be done in a separate thread.
	       // When done, remember that all registration is done.
    		
    		httpRequest(Utilities.server + "/register?id=" + registration);

	    }
	}

	private void handleMessage(Context context, Intent intent)
	{
	
		//httpRequest(server + "/hello");
		Log.v("RoboLock", "CD2M SUCCESS!");
		
		Bundle extras = intent.getExtras();
		if (extras != null) {
			//parse the message and do something with it.
			//For example, if the server sent the payload as "data.message=xxx", here you would have an extra called "message"
			String message = extras.getString("payload");

			serverNotification(context);				

		}
	}

	void sendIntentToPhotoView() {
		
	}
	
	void httpRequest(String url) {
		final DefaultHttpClient client = new DefaultHttpClient();
		final HttpGet getRequest = new HttpGet(url);

		try {
			HttpResponse response = client.execute(getRequest);
			int statusCode = response.getStatusLine().getStatusCode();
			if (statusCode != HttpStatus.SC_OK) {
				Log.w("ImageDownloader [0]", "Error " + statusCode
						+ " while retrieving bitmap from " + url);
			}

			final HttpEntity entity = response.getEntity();
			if (entity != null) {


			}
		} catch (Exception e) {
			// Could provide a more explicit error message for IOException or
			// IllegalStateException
			getRequest.abort();
			Log.v("ImageDownloader [1]", "Error while retrieving bitmap from "
					+ url + e.toString());
		}
	}
	
	
	public void serverNotification(Context context) {
		String ns = Context.NOTIFICATION_SERVICE;
	    NotificationManager nm =
	        (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
		
		int icon = R.drawable.ic_robolock; // TODO will change this
		CharSequence tickerText = "Someone is at your door";
		long when = System.currentTimeMillis();
	
		Notification notification = new Notification(icon, tickerText, when);
		
		notification.flags = Notification.FLAG_AUTO_CANCEL;
		
		CharSequence contentTitle = "RoboLock";
		CharSequence contentText = "Someone is at your door";
		
		  Intent notificationIntent = new Intent(context, RoboLock.class)
          	.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);  

		PendingIntent contentIntent = PendingIntent
			.getActivity(context, 0, notificationIntent, 0);
	
		notification.setLatestEventInfo(context, contentTitle, contentText, contentIntent);
		notification.sound = Uri.parse("file:///sdcard/robolock/doorbell.mp3");
		
		int HELLO_ID = 1;
	
		nm.notify(HELLO_ID, notification);



		
	}	

	
	
}