#pragma once

struct ScriptedBehaviourComponent {
	std::shared_ptr <IScriptedBehaviour> script;

	ScriptedBehaviourComponent(const std::shared_ptr <IScriptedBehaviour>& s = nullptr) : script(s) {
		//this->script = script;
	}
};