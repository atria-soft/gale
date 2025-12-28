package org.atriasoft.gale.context;

import java.util.ArrayList;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class CommandLine {
	static final Logger LOGGER = LoggerFactory.getLogger(CommandLine.class);
	private final List<String> listArgs = new ArrayList<>();
	
	public void parse(final String[] args) {
		for (int iii = 1; iii < args.length; iii++) {
			LOGGER.debug("commandLine: '{}'", args[iii]);
			this.listArgs.add(args[iii]);
		}
	}

	public int size() {
		return this.listArgs.size();
	}

	public String get(final int id) {
		return this.listArgs.get(id);
	}

	public void add(final String newElement) {
		this.listArgs.add(newElement);
	}

	public void remove(final int id) {
		this.listArgs.remove(id);
	}
}
