package hyrax.v2.personfinder;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;

import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.text.InputFilter.LengthFilter;
import android.util.Log;
import android.view.Menu;
import android.widget.Toast;
import au.com.bytecode.opencsv.CSVWriter;

public class MainActivity extends Activity {

	public String LOG_TAG="personfinder";
	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS:
			{
				Log.i(LOG_TAG, "OpenCV loaded successfully");
				System.loadLibrary("detector");
			} break;
			default:
			{
				super.onManagerConnected(status);
			} break;
			}
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		File files = new File("/mnt/sdcard/external_sd/hyraxv2/testImages/");
		File[] list = files.listFiles();

		int lists = files.listFiles().length;
		//String qwe = list.toString();

		Log.d(LOG_TAG, "list: " + lists);
		Log.d(LOG_TAG, Arrays.toString(list));
		//Toast t= Toast.makeText(getApplicationContext(), LOG_TAG, Toast.LENGTH_LONG);
		// t.show();
		String tempNames=new String(Arrays.toString(list));
		tempNames=tempNames.replace("[", "");
		tempNames=tempNames.replace("]", "");
		String[] fileNames=tempNames.split(",");
		CSVWriter writer = null;
		try 
		{
			writer = new CSVWriter(new FileWriter("/mnt/sdcard/external_sd/hyraxv2/testImages.csv"),'\n');
			writer.writeNext(fileNames);
			writer.close();
		} 

		catch (IOException e)
		{
			Toast p= Toast.makeText(getApplicationContext(),"Could not write to the SD", Toast.LENGTH_LONG);
			p.show();
		}
		detector(1);
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	private static native void detector(int sampleLabel);
}