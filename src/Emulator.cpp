#include "Emulator.h"
#include "CPUOpcodes.h"
#include "Log.h"

NAMESPACE_BEGIN(sn)

Emulator::Emulator(): 
	m_cpu(m_bus), 
	m_ppu(m_pictureBus, m_emulatorScreen), 
	m_screenScale(3.f), 
	m_cycleTimer(), 
	m_cpuCycleDuration(NanoSeconds(559)) {
	// set callback function pointer
	if(!m_bus.setReadCallback(PPUSTATUS, [&](void) {return m_ppu.getStatus();}) 
		|| !m_bus.setReadCallback(PPUDATA, [&](void) {return m_ppu.getData();}) 
		|| !m_bus.setReadCallback(JOY1, [&](void) {return m_controller1.read();}) 
		|| !m_bus.setReadCallback(JOY2, [&](void) {return m_controller2.read();}) 
		|| !m_bus.setReadCallback(OAMDATA, [&](void) {return m_ppu.getOAMData();})) {
		_error("Critical error: Failed to set I/O callbacks");
	}

	if(!m_bus.setWriteCallback(PPUCTRL, [&](Byte b) {m_ppu.control(b);}) 
		|| !m_bus.setWriteCallback(PPUMASK, [&](Byte b) {m_ppu.setMask(b);}) 
		|| !m_bus.setWriteCallback(OAMADDR, [&](Byte b) {m_ppu.setOAMAddress(b);}) 
		|| !m_bus.setWriteCallback(PPUADDR, [&](Byte b) {m_ppu.setDataAddress(b);}) 
		|| !m_bus.setWriteCallback(PPUSCROL, [&](Byte b) {m_ppu.setScroll(b);}) 
		|| !m_bus.setWriteCallback(PPUDATA, [&](Byte b) {m_ppu.setData(b);}) 
		|| !m_bus.setWriteCallback(OAMDMA, [&](Byte b) {DMA(b);}) 
		|| !m_bus.setWriteCallback(JOY1, [&](Byte b) {m_controller1.strobe(b); m_controller2.strobe(b);}) 
		|| !m_bus.setWriteCallback(OAMDATA, [&](Byte b) {m_ppu.setOAMData(b);})) {
		_error("Critical error: Failed to set I/O callbacks");
	}

	m_ppu.setInterruptCallback([&](){ m_cpu.interrupt(InterruptType::NMI); });
}

void Emulator::run(std::string rom_path) {
	if (!m_cartridge.loadFromFile(rom_path)) {
		return;
	}

	m_mapper = Mapper::createMapper(
		static_cast<MapperType>(m_cartridge.getMapper()),
		m_cartridge,
		[&](){ m_cpu.interrupt(InterruptType::IRQ); },
		[&](){ m_pictureBus.updateMirroring(); }
	);
		
	if (!m_mapper) {
		_error("Creating Mapper failed. Probably unsupported.");
		return;
	}

	if (!m_bus.setMapper(m_mapper.get()) || !m_pictureBus.setMapper(m_mapper.get())) {
		return;
	}

	m_cpu.reset();
	m_ppu.reset();

	m_window.create(
		sf::VideoMode(NESVideoWidth * m_screenScale, NESVideoHeight * m_screenScale),
		"NesBoy", 
		sf::Style::Titlebar|sf::Style::Close|sf::Style::Resize
	);
	
	m_window.setVerticalSyncEnabled(true);
	m_emulatorScreen.create(NESVideoWidth, NESVideoHeight, m_screenScale, sf::Color::White);

	m_cycleTimer = std::chrono::high_resolution_clock::now();
	m_elapsedTime = m_cycleTimer - m_cycleTimer;

	sf::Event event;
	bool focus = true, pause = false;

	while (m_window.isOpen()) {
		while (m_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				m_window.close();
				return;
			} else if (event.type == sf::Event::GainedFocus) {
				focus = true;
				m_cycleTimer = std::chrono::high_resolution_clock::now();
			} else if (event.type == sf::Event::LostFocus) {
				focus = false;
			} else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2) {
				pause = !pause;
				if (!pause) {
					m_cycleTimer = std::chrono::high_resolution_clock::now();
					_info("Paused.");
				} else {
					_info("Unpaused.");
				}
			} else if (pause && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F3) {
				for (int i = 0; i < 29781; ++i) { //Around one frame
					//PPU
					m_ppu.step();
					m_ppu.step();
					m_ppu.step();
					//CPU
					m_cpu.step();
				}
			} 
		}

		if (focus && !pause) {
			m_elapsedTime += std::chrono::high_resolution_clock::now() - m_cycleTimer;
			m_cycleTimer = std::chrono::high_resolution_clock::now();

			while (m_elapsedTime > m_cpuCycleDuration) {
				//PPU
				m_ppu.step();
				m_ppu.step();
				m_ppu.step();
				//CPU
				m_cpu.step();
				m_elapsedTime -= m_cpuCycleDuration;
			}

			m_window.draw(m_emulatorScreen);
			m_window.display();
		} else {
			sf::sleep(sf::milliseconds(1000/60));
			//std::this_thread::sleep_for(std::chrono::milliseconds(1000/60)); //1/60 second
		}
	}
}

void Emulator::DMA(Byte page) {
	m_cpu.skipDMACycles();
	auto page_ptr = m_bus.getPagePtr(page);
	if (page_ptr != nullptr) {
		m_ppu.doDMA(page_ptr);
	} else {
		_error("Can't get pageptr for DMA");
	}
}

void Emulator::setVideoHeight(int height) {
	m_screenScale = height / float(NESVideoHeight);
	_info("Scale: {} set. Screen: {} x {}", m_screenScale, int(NESVideoWidth*m_screenScale), int(NESVideoHeight*m_screenScale));
}

void Emulator::setVideoWidth(int width) {
	m_screenScale = width / float(NESVideoWidth);
	_info("Scale: {} set. Screen: {} x {}", m_screenScale, int(NESVideoWidth*m_screenScale), int(NESVideoHeight*m_screenScale));

}

void Emulator::setVideoScale(float scale) {
	m_screenScale = scale;
	_info("Scale: {} set. Screen: {} x {}", m_screenScale, int(NESVideoWidth*m_screenScale), int(NESVideoHeight*m_screenScale));
}

void Emulator::setKeys(std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2) {
	m_controller1.setKeyBindings(p1);
	m_controller2.setKeyBindings(p2);
}

NAMESPACE_END(sn)
