package org.atriasoft.gale.context;

import org.atriasoft.gale.GaleApplication;

class GaleApplicationTest extends GaleApplication {
	
}

public class GaleContextTest extends GaleContext {
	
	public GaleContextTest() {
		super(new GaleApplicationTest(), new String[0]);
		setContext(this);
	}
	
	@Override
	public int run() {
		// TODO Auto-generated method stub
		return 0;
	}
	
}
