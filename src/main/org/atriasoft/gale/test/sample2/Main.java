package org.atriasoft.gale.test.sample2;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.Gale;
import org.atriasoft.gale.test.sample1.Sample1;

public class Main {
	public static void main(final String[] args) {
		Gale.init();
		Uri.setApplication(Sample1.class, "/org/atriasoft/gale/test/sample2/");
		Gale.run(new Sample2Application(), args);
	}
	
	private Main() {}
}
