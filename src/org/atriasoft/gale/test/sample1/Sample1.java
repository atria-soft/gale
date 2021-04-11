package org.atriasoft.gale.test.sample1;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.Gale;

public class Sample1 {
	public static void main(final String[] args) {
		Gale.init();
		//Uri.setGroup("DATA", "src/org/atriasoft/gale/test/sample1/");
		Uri.setApplication(Sample1.class, "/org/atriasoft/gale/test/sample1/");
		Gale.run(new Sample1Application(), args);
	}
	
	private Sample1() {}
}
