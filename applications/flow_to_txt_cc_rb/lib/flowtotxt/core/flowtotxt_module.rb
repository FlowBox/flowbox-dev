module FlowToTxt
  module Core
    class FlowToTxtModule
      # configuration
      def configure(configure_h)

        conf_check_input(configure_h)
        conf_check_output(configure_h)
        conf_check_stat_interval(configure_h)

        conf_push(configure_h)
      end

      def conf_check_input(configure_h)
        input = configure_h[:input]
        case input
        when nil
          raise('no input buffer found tag found')
        when 'null'
          raise('we need an input (null) not accepted')
        end
      end

      def conf_check_output(configure_h)
        output = configure_h[:output]
        case output
        when nil
          raise('no output buffer found tag found')
        end
      end

      def conf_check_stat_interval(configure_h)
        stat_interval_s = configure_h[:stat_interval_s]
        case stat_interval_s
        when nil
          raise('no stat_interval_s found tag found')
        when stat_interval_s < 0
         raise('kidding? stat_interval_s > 0')
        end
      end
    end
  end
end

