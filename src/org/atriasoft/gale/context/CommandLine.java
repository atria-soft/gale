package org.atriasoft.gale.context;

import java.util.ArrayList;
import java.util.List;

import org.atriasoft.gale.internal.Log;

public class CommandLine {
	private List<String> listArgs = new ArrayList<String>();

	public void parse(String[] args) {
		for (int iii=1 ; iii<args.length; iii++) {
			Log.info("commandLine : '" + args[iii] + "'" );
			listArgs.add(args[iii]);
		}
	}
	
	public int size() {
		return listArgs.size();
	}
	
	public String get(int id) {
		return listArgs.get(id);
	}
	
	public void add(String newElement) {
		listArgs.add(newElement);
	}
	
	public void remove(int id) {
		listArgs.remove(id);
	}
}
