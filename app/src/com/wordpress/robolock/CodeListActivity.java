package com.wordpress.robolock;

import android.app.AlertDialog;
import android.app.ListActivity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.InputFilter;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class CodeListActivity extends ListActivity {

	final Context ctxt = this;
	String[] codes;
	ProgressDialog pd;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		pd = ProgressDialog.show(this, "Contacting RoboLock", "Please Wait...", true, false);
		new FetchCodes().execute(Utilities.server + "/getcodes");
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.codemenu, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle item selection
		switch (item.getItemId()) {
		case R.id.refresh:
			refresh();
			return true;
		case R.id.addcode:
			addCode();
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	public void addCode() {
		AlertDialog.Builder alert = new AlertDialog.Builder(this);

		alert.setTitle("Add Code");

		InputFilter[] FilterArray = new InputFilter[1];
		FilterArray[0] = new InputFilter.LengthFilter(Utilities.CODE_LENGTH);

		// Set an EditText view to get user input
		final EditText input = new EditText(this);
		input.setRawInputType(Configuration.KEYBOARD_12KEY);
		input.setFilters(FilterArray);

		alert.setView(input);

		alert.setPositiveButton("Add", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				String value = input.getText().toString();
				
				if (value.length() == Utilities.CODE_LENGTH) {
				
					Utilities.httpRequest(Utilities.server + "/setcode?code=" + value);
					refresh();
				} else {
					int duration = Toast.LENGTH_SHORT;

					Toast toast = Toast.makeText(ctxt, "Code too short!", duration);
					toast.show();

					addCode();
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

	public void refresh() {
		Intent intent = new Intent(CodeListActivity.this,
				CodeListActivity.class);
		CodeListActivity.this.startActivity(intent);
		finish();
	}

	public void buildListView() {

		setListAdapter(new ArrayAdapter<String>(this, R.layout.code_list, codes));

		ListView listView = getListView();
		listView.setTextFilterEnabled(true);

		listView.setOnItemClickListener(new OnItemClickListener() {

			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {

				AlertDialog.Builder alert = new AlertDialog.Builder(ctxt);
				final String code = ((TextView) view).getText().toString();
				alert.setTitle("Delete Code");
				alert.setMessage("Do you want to delete \"" + code + "\"?");

				alert.setPositiveButton("Delete", new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								Utilities.httpRequest(Utilities.server + "/invalidate?code=" + code);
								refresh();
							}
						});

				alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								// Canceled.
							}
						});

				alert.show();
			}
		});
	}

	private class FetchCodes extends AsyncTask<String, Integer, Long> {
		protected Long doInBackground(String... url) {
			codes = Utilities.httpRequest(url[0]).split("[\n]");
			return 0L;
		}

		protected void onProgressUpdate(Integer... progress) {
		}

		protected void onPostExecute(Long result) {

			if (codes == null) {
				finish();
			} else {
				buildListView();
			}

			pd.cancel();
		}
	}

}
