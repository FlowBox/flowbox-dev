module FlowToTxt
  module Driver
    module Console
      class FlowToTxtModule
        attr_reader :finished
        def initialize()
          @finished = false
        end
        def configure(configure_h)
          @core ||= FlowToTxt::Core::FlowToTxtModule.new()
          @finished = false
          @core.configure(configure_h)
        end
        def start()
          @worker_stat = Thread.new{worker_stat_do()}
          @worker_obs = Thread.new{worker_obs_do()} 
          @worker_data = Thread.new{worker_data_do()} 
        end
        def worker_obs_do
          while true
            answer = @core.obs_get()
            if answer == nil
              # e.g. blocking was canceled due to an interrupt 
              puts "FlowToTxtModule::worker_obs_due: Interrupt observed"
            else
              answer.each do |key, data|
                puts "#{key}, #{data}"
              end
              STDOUT.flush()
            end
          end
        end
        def worker_stat_do
          while true
            answer = @core.stat_get()
            if answer == nil
              # e.g. blocking was canceled due to an interrupt 
              puts "FlowToTxtModule::worker_stat_due: Interrupt observed"
            else
              puts "FlowToTxtModule: #{answer['time_s']}, #{answer['flows']}"
              STDOUT.flush()
            end
          end
        end
        def worker_data_do
          @core.data_process()
          @finished = true
        end
      end # FlowToTxtModule
    end # Console
  end # Driver
end # FlowToTxt

