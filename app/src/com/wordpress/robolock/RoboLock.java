package com.wordpress.robolock;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

public class RoboLock extends Activity {

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	//	cd2mSetup();
	}
	
	public void cd2mSetup() {
		Intent registrationIntent = new Intent("com.google.android.c2dm.intent.REGISTER");
		registrationIntent.putExtra("app", PendingIntent.getBroadcast(this, 0, new Intent(), 0)); // boilerplate
		registrationIntent.putExtra("sender", "sender@email.com");
		startService(registrationIntent);
		Log.v("RoboLock", "Starting service...");
	}

	public void unlock(View v) {
		Utilities.httpRequest(Utilities.server + "/unlock");
	}
	
	public void loadImage(View v) {
		fetchImage(Utilities.server + "/photo");
	}
	
	public void manage(View v) {
		Intent intent = new Intent(RoboLock.this, CodeListActivity.class);
		RoboLock.this.startActivity(intent);
	}
	
	void fetchImage(String s) {
		Bitmap image = Utilities.bitmapDownloader(s);
		ImageView imview = (ImageView) findViewById(R.id.photo);
		imview.setImageBitmap(image);
	}
		
}






