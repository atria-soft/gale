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

import org.junit.jupiter.api.MethodOrderer.OrderAnnotation;
import org.junit.jupiter.api.Order;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestMethodOrder;

@TestMethodOrder(OrderAnnotation.class)
public class TestBasicLog {
	// JUST keep a kanva ...
	@Test
	@Order(1)
	public void aaFirstInitialisation() {
		final List<String> args = new ArrayList<>();
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
	}

}
