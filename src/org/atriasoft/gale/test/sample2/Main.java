package org.atriasoft.gale.test.sample2;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.Gale;

public class Main {
	private Main() {}
	public static void main(String[] args) {
		Uri.setGroup("DATA", "src/org/atriasoft/gale/test/sample2/");
		Gale.run(new Sample2Application(), args);
	}
}
