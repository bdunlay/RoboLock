package com.wordpress.robolock;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;

public class RoboLock extends Activity {

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		// cd2mSetup();
		loadImage();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.servermenu, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle item selection
		switch (item.getItemId()) {
		case R.id.remove:
			remove();
			return true;
		case R.id.take:
			takePhoto(null);
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}
	
	
	public void cd2mSetup() {
		Intent registrationIntent = new Intent(
				"com.google.android.c2dm.intent.REGISTER");
		registrationIntent.putExtra("app",
				PendingIntent.getBroadcast(this, 0, new Intent(), 0)); // boilerplate
		registrationIntent.putExtra("sender", "sender@email.com");
		startService(registrationIntent);
		Log.v("RoboLock", "Starting service...");
	}

	public void unlock(View v) {
		Utilities.httpRequest(Utilities.server + "/unlock");
	}
	
	public void remove() {
		Utilities.httpRequest(Utilities.server + "/move");
	}


	// this should be async task which updates the photo when ready
	// TODO make the notification indicate that there is a new image to load
	// TODO only send notification once image has been completed
	// TODO make different notifications for alerts and on-demand (on-demand wont open a notification)
	public void loadImage() {
		fetchImage(Utilities.server + "/photo");
	}
	
	// this should send a request to take a new photo and update the photo when a notification is received
	public void takePhoto(View v) {
		Utilities.httpRequest(Utilities.server + "/takephoto");
	}

	public void manage(View v) {
		Intent intent = new Intent(RoboLock.this, CodeListActivity.class);
		RoboLock.this.startActivity(intent);
	}

	public void greeting(View v) {

		AlertDialog.Builder alert = new AlertDialog.Builder(this);

		alert.setTitle("Set Text Greeting");

		// Set an EditText view to get user input
		final EditText input = new EditText(this);

		alert.setView(input);

		alert.setPositiveButton("Send", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				String value = input.getText().toString();
				try {
					
					Utilities.httpRequest(Utilities.server + "/greeting?msg="
							+ URLEncoder.encode(value, "utf-8"));

				} catch (UnsupportedEncodingException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		});

		alert.setNegativeButton("Cancel",
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						// cancelled
					}
				});

		alert.show();

	}

	void fetchImage(String s) {
		Bitmap image = Utilities.bitmapDownloader(s);
		ImageView imview = (ImageView) findViewById(R.id.photo);
		imview.setImageBitmap(image);
	}
	


}
