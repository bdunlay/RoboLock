package com.wordpress.robolock;

import java.io.InputStream;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences.Editor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

public class Main extends Activity {

	
	final String server = "http://192.168.1.149:8080";

	
	
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
//		cd2mSetup();

	}
	
	public void cd2mSetup() {

		Intent registrationIntent = new Intent("com.google.android.c2dm.intent.REGISTER");
		registrationIntent.putExtra("app", PendingIntent.getBroadcast(this, 0, new Intent(), 0)); // boilerplate
		registrationIntent.putExtra("sender", "sender@email.com");
		startService(registrationIntent);
		Log.v("RoboLock", "Starting service...");
	}

	public void onReceive(Context context, Intent intent) {
	    if (intent.getAction().equals("com.google.android.c2dm.intent.REGISTRATION")) {
	        handleRegistration(context, intent);
	    } else if (intent.getAction().equals("com.google.android.c2dm.intent.RECEIVE")) {
	        handleMessage(context, intent);
	     }
	    
		Log.v("RoboLock", "Something received...");

	 }
	
	private void handleRegistration(Context context, Intent intent) {
		Log.v("RoboLock", "Handling registration...");

		
	    String registration = intent.getStringExtra("registration_id"); 
	    if (intent.getStringExtra("error") != null) {
	        // Registration failed, should try again later.
	    	
	    	
			Context c = getApplicationContext();
			int duration = Toast.LENGTH_SHORT;

			Toast toast = Toast.makeText(c,
					"Failed", duration);
			toast.show();

	    	
	    } else if (intent.getStringExtra("unregistered") != null) {
	        // unregistration done, new messages from the authorized sender will be rejected
	    	
			Context c = getApplicationContext();
			int duration = Toast.LENGTH_SHORT;

			Toast toast = Toast.makeText(c,
					"Unregistered", duration);
			toast.show();

	    	
	    } else if (registration != null) {
	       // Send the registration ID to the 3rd party site that is sending the messages.
	       // This should be done in a separate thread.
	       // When done, remember that all registration is done. 
	    	
	    	httpRequest(server + "/register?id=" + registration);
	    	
	    	
	    }
	}

	private void handleMessage(Context context, Intent intent) {
	    String message = intent.getExtras().getString("Message");
	    
	    if (message.toUpperCase() == "REQUEST") {
			Context c = getApplicationContext();
			int duration = Toast.LENGTH_SHORT;

			Toast toast = Toast.makeText(c,
					"Someone is at your door", duration);
			toast.show();
	    }

	}

	
	public void openDoor(View v) {
		httpRequest(server);
	}
	
	public void loadImage(View v) {
		fetchImage("http://www.torreytrust.com/images/ucsb.jpg");
	}
	
	void fetchImage(String s) {
		Bitmap image = bitmapDownloader(s);
		ImageView imview = (ImageView) findViewById(R.id.photo);
		imview.setImageBitmap(image);
	}


	public Bitmap bitmapDownloader(String url) {
		final DefaultHttpClient client = new DefaultHttpClient();
		final HttpGet getRequest = new HttpGet(url);

		try {
			HttpResponse response = client.execute(getRequest);
			final int statusCode = response.getStatusLine().getStatusCode();
			if (statusCode != HttpStatus.SC_OK) {
				Log.w("ImageDownloader [0]", "Error " + statusCode
						+ " while retrieving bitmap from " + url);
				return null;
			}

			final HttpEntity entity = response.getEntity();
			if (entity != null) {

				InputStream inputStream = null;

				try {
					inputStream = entity.getContent();
					final Bitmap bitmap = BitmapFactory
							.decodeStream(inputStream);
					return bitmap;
				} finally {
					if (inputStream != null) {
						inputStream.close();
					}
					entity.consumeContent();
				}
			}
		} catch (Exception e) {
			// Could provide a more explicit error message for IOException or
			// IllegalStateException
			getRequest.abort();
			Log.v("ImageDownloader [1]", "Error while retrieving bitmap from "
					+ url + e.toString());
		} finally {
		}
		return null;
	}

	// this is a duplicate function of the above code, but whatever
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

				Context context = getApplicationContext();
				int duration = Toast.LENGTH_SHORT;

				Toast toast = Toast.makeText(context,
						Integer.toString(statusCode), duration);
				toast.show();
			}
		} catch (Exception e) {
			// Could provide a more explicit error message for IOException or
			// IllegalStateException
			getRequest.abort();
			Log.v("ImageDownloader [1]", "Error while retrieving bitmap from "
					+ url + e.toString());
		}
	}
}
