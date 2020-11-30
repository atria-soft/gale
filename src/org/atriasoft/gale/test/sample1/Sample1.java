package org.atriasoft.gale.test.sample1;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.Gale;

public class Sample1 {
	private Sample1() {}
	public static void main(String[] args) {
		Uri.setGroup("DATA", "src/org/atriasoft/gale/test/sample1/");
		Gale.run(new Sample1Application(), args);
	}
}
