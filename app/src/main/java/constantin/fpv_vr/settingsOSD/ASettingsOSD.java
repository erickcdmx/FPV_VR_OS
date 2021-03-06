package constantin.fpv_vr.settingsOSD;

import android.os.Bundle;
import android.util.Log;
import androidx.appcompat.app.AppCompatActivity;
import constantin.fpv_vr.R;

import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceManager;
import androidx.preference.PreferenceScreen;

public class ASettingsOSD extends AppCompatActivity implements PreferenceFragmentCompat.OnPreferenceStartScreenCallback{
    private static final String TAG= ASettingsOSD.class.getSimpleName();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Display the fragment as the main content.
        getSupportFragmentManager().beginTransaction()
                .replace(android.R.id.content, new FSettingsOSD())
                .commit();
    }

    @Override
    protected void onPause(){
        super.onPause();
    }


    @Override
    public boolean onPreferenceStartScreen(PreferenceFragmentCompat caller, PreferenceScreen preferenceScreen) {
        final FSettingsOSD newSettingsFragment=new FSettingsOSD();
        //if(preferenceScreen.getKey()==null){
        //    preferenceScreen.setKey("lalu");
        //}
        Bundle args = new Bundle();
        args.putString(PreferenceFragmentCompat.ARG_PREFERENCE_ROOT,preferenceScreen.getKey());
        newSettingsFragment.setArguments(args);

        getSupportFragmentManager().beginTransaction()
                .replace(android.R.id.content, newSettingsFragment)
                .addToBackStack(null)
                .commit();
        Log.d(TAG,"onPreferenceStartScreen "+preferenceScreen.getKey());
        return false;
    }

    public static class FSettingsOSD extends PreferenceFragmentCompat{
        // If created by the custom constructor taking a PreferenceScreen the fragment will be inflated using this preferenceScreen instead
        // Workaround for nested preference screens inside .xml file
        /*private final PreferenceScreen preferenceScreen;
        FSettingsOSD(PreferenceScreen preferenceScreen){
            this.preferenceScreen=preferenceScreen;
        }
        FSettingsOSD(){
            this.preferenceScreen=null;
        }*/

        @Override
        public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
            PreferenceManager preferenceManager=getPreferenceManager();
            preferenceManager.setSharedPreferencesName("pref_osd");
            //if(preferenceScreen==null){
                //addPreferencesFromResource(R.xml.pref_osd_elements);
                //setPreferencesFromResource(R.xml.pref_osd_elements,rootKey);
                addPreferencesFromResource(R.xml.pref_osd_style);
                addPreferencesFromResource(R.xml.pref_osd_elements);
                if(rootKey!=null){
                    final Preference root=getPreferenceScreen().findPreference(rootKey);
                    setPreferenceScreen((PreferenceScreen)root);
                }
                //setPreferencesFromResource(R.xml.pref_osd_style,rootKey);
                //setPreferencesFromResource(R.xml.pref_osd_elements,rootKey);
            //}else{
            //    setPreferenceScreen(preferenceScreen);
            //}
        }
    }

}
