/*******************************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Contributors:
 *     DUPIN Edouard - initial API and implementation
 ******************************************************************************/
package test.atriasoft.gale;

import java.util.ArrayList;
import java.util.List;

import io.scenarium.logger.Logger;

import org.junit.Test;
import org.junit.jupiter.api.MethodOrderer.OrderAnnotation;
import org.junit.jupiter.api.Order;
//import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestMethodOrder;

@TestMethodOrder(OrderAnnotation.class)
public class TestBasicLog {

	@Test
	@Order(1)
	public void aaFirstInitialisation() {
		List<String> args = new ArrayList<>();
		args.add("--log-level=999");
		args.add("--log-level=1");
		args.add("--log-no-color");
		args.add("--log-color");
		args.add("--log-lib=sc-log-test+6");
		args.add("--log-lib=sc-log-test/6");
		args.add("--log-lib=sc-log-test:6");
		args.add("--log-lib=sc-log-test:verbose");
		args.add("--log-lib=sc-log-test2+3");
		args.add("--log-lib=sc-log-test");
		args.add("--log-with-stupid-parameter=sdkfjsqdlkf");
		args.add("--help");
		Logger.init(args);
	}

	@Test
	@Order(2)
	public void bbSecondInitialisation() {
		List<String> args = new ArrayList<>();
		Logger.init(args);
	}

	@Test
	@Order(3)
	public void ccBasicLogCall() {
		Log.print("Simple print");
		Log.todo("Simple todo");
		Log.error("Simple error");
		Log.warning("Simple warning");
		Log.info("Simple info");
		Log.debug("Simple debug");
		Log.verbose("Simple verbose");
	}

	// TODO REFACTO REMOVE this and set it in the Test of the logger.
	public static String getAAAAAAA(int dfsdf) {
		int hhh = 0;
		for (int kkk = 0; kkk < dfsdf; kkk++)
			for (int iii = 0; iii < 10000; iii++)
				for (int jjj = 0; jjj < 100000; jjj++)
					for (int lll = 0; lll < 100000; lll++)
						hhh++;
		return "kkk" + hhh;
	}

	public static void testLog() {
		Log.print("test direct [START]");
		// test de 10 secondes contre 0.0?? second quand le niveau n'est pas assez grand ...
		long timeStart = System.currentTimeMillis();
		for (int iii = 0; iii < 100000000; iii++)
			Log2.debug("test direct");
		long timeStop = System.currentTimeMillis();
		Log.print("test direct [END] : " + timeStart + " to " + timeStop + "    ==> delta=" + (timeStop - timeStart));
		Log.print("test concat [START]");
		// C'est très long dans les 2 cas ...
		timeStart = System.currentTimeMillis();
		for (int iii = 0; iii < 6; iii++)
			Log2.debug("test concat: non fonctionnel, il applelle le get a chaque log ... " + getAAAAAAA(iii));
		timeStop = System.currentTimeMillis();
		Log.print("test concat [END] : " + timeStart + " to " + timeStop + "    ==> delta=" + (timeStop - timeStart));
	}

	@Test
	@Order(4)
	public void ddTestSimpleLog() {
		testLog();
	}

	@Test
	@Order(4)
	public void eeUsage() {
		Logger.usage();
	}

}
